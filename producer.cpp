#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <amqpcpp/linux_tcp.h>
#include <ev.h>

#include <fstream>
#include <iostream>
#include <string>

int main() {
    // Создание цикла событий libev
    struct ev_loop* evloop = EV_DEFAULT;

    // Создание соединения с RabbitMQ
    AMQP::Address address("amqp://vladislav:hard_password@rabbitmq:5672/");
    AMQP::LibEvHandler handler(evloop);
    AMQP::TcpConnection connection(&handler, address);
    AMQP::TcpChannel channel(&connection);

    // Открываем файл data.txt
    std::ifstream file("data.txt");
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл data.txt" << std::endl;
        return 1;
    }

    // Создание очереди и отправка данных
    channel.declareQueue("data_queue", AMQP::durable)
        .onSuccess([&channel, &file, evloop](const std::string &name, uint32_t, uint32_t) {
            std::string line;
            while (std::getline(file, line)) {
                channel.publish("", name, line); // Отправка строки в очередь
                std::cout << "Отправлено: " << line << std::endl;
            }
            file.close();

            // Отправка сообщения-заглушки "END"
            channel.publish("", name, "END");
            std::cout << "Отправлено сообщение-заглушка 'END'" << std::endl;

            // Завершение цикла событий
            std::cout << "Все сообщения отправлены. Завершение работы producer." << std::endl;
            ev_break(evloop, EVBREAK_ALL);
            return;
        })
        .onError([](const char *message) {
            std::cerr << "Ошибка при отправке данных: " << message << std::endl;
        });

    ev_run(evloop, 0);

    return 0;
}