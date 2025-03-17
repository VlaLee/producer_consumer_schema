#!/bin/bash

# Очищаем директорию output/
sudo rm -r output/

# Парсинг аргументов командной строки
SHOW_LOGS=false
CONSUMER_REPLICAS=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --logs)
            SHOW_LOGS=true
            shift
            ;;
        *)
            if [[ -z "$CONSUMER_REPLICAS" ]]; then
                CONSUMER_REPLICAS=$1
                shift
            else
                echo "Неизвестный аргумент: $1"
                exit 1
            fi
            ;;
    esac
done

# Проверка наличия аргумента
if [[ -z "$CONSUMER_REPLICAS" ]]; then
    echo "Использование: $0 [--logs] <количество_consumer-ов>"
    exit 1
fi

# Запуск всех контейнеров
echo "Запуск контейнеров с $CONSUMER_REPLICAS consumer-ами..."
sudo CONSUMER_REPLICAS=$CONSUMER_REPLICAS docker-compose up --build -d

# Вывод логов в реальном времени, если указан флаг --logs
if [[ "$SHOW_LOGS" == true ]]; then
    echo "Вывод логов в реальном времени..."
    docker-compose logs -f &
fi

# Засекаем время начала
start_time=$(date +%s)
echo "Таймер запущен..."

# Ожидание завершения работы producer и consumer
echo "Ожидание завершения работы producer и consumer..."
while true; do
    # Проверяем состояние контейнера producer
    producer_status=$(docker inspect -f '{{.State.Status}}' producer 2>/dev/null)

    # Проверяем состояние всех consumer-ов
    all_consumers_exited=true
    for ((i=1; i<=CONSUMER_REPLICAS; i++)); do
        consumer_status=$(docker inspect -f '{{.State.Status}}' "consumer_schema-consumer-$i" 2>/dev/null)
        if [[ "$consumer_status" != "exited" ]]; then
            all_consumers_exited=false
            break
        fi
    done

    # Если producer и все consumer завершены, выходим из цикла
    if [[ "$producer_status" == "exited" ]] && $all_consumers_exited; then
        break
    fi

    sleep 1
done

# Засекаем время окончания
end_time=$(date +%s)

# Остановка RabbitMQ
echo "Все контейнеры producer и consumer завершены. Останавливаю RabbitMQ..."
docker-compose down

# Вычисляем общее время выполнения
total_time=$((end_time - start_time))
echo "Общее время выполнения системы с $CONSUMER_REPLICAS consumer-ами: $total_time секунд."