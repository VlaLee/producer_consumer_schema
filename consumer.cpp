#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <amqpcpp/linux_tcp.h>
#include <ev.h>

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include <stdexcept>
#include <chrono>
#include <thread>

int main() {
    // Создание цикла событий libev
    struct ev_loop* evloop = EV_DEFAULT;

    // Создание соединения с RabbitMQ
    AMQP::Address address("amqp://vladislav:hard_password@rabbitmq:5672/");
    AMQP::LibEvHandler handler(evloop);
    AMQP::TcpConnection connection(&handler, address);
    AMQP::TcpChannel channel(&connection);

    // Ограничение количества сообщений, которые потребитель получает за раз
    channel.setQos(1);

    // Получение данных из очереди
    channel.declareQueue("data_queue", AMQP::durable)
        .onSuccess([&channel, evloop](const std::string &name, uint32_t, uint32_t) {
            channel.consume(name).onReceived([&channel, evloop](const AMQP::Message &message,
                                                                    uint64_t deliveryTag, bool) {
                try {
                    std::string data(message.body(), message.bodySize());

                    // Проверка на сообщение-заглушку
                    if (data == "END") {
                        std::cout << "Получено сообщение-заглушка 'END'. Завершение работы." << std::endl;
                        ev_break(evloop, EVBREAK_ALL);  // Завершение цикла событий
                        return;
                    }

                    size_t space1 = data.find(' ');
                    size_t space2 = data.find(' ', space1 + 1);
                    size_t space3 = data.find(' ', space2 + 1);

                    if (space1 == std::string::npos || space2 == std::string::npos || space3 == std::string::npos) {
                        throw std::runtime_error("Некорректный формат данных: " + data);
                    }

                    std::string surname = data.substr(space1 + 1, space2 - space1 - 1);
                    char firstLetter = std::toupper(surname[0]);

                    std::ofstream outFile("output/" + std::string(1, firstLetter) + ".txt", std::ios::app);
                    if (!outFile.is_open()) {
                        throw std::runtime_error("Ошибка: не удалось открыть файл для буквы " + std::string(1, firstLetter));
                    }
                    outFile << data << std::endl;
                    outFile.close();
                    std::cout << "Данные сохранены в файл: " << firstLetter << ".txt" << std::endl;

                    // Подтверждение обработки сообщения
                    channel.ack(deliveryTag);

                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                } catch (const std::exception &e) {
                    std::cerr << "Ошибка при обработке сообщения: " << e.what() << std::endl;
                }
            });
        })
        .onError([](const char *message) {
            std::cerr << "Ошибка при получении данных: " << message << std::endl;
        });

    ev_run(evloop, 0);

    return 0;
}