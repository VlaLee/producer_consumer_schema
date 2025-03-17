import matplotlib.pyplot as plt

with open('results.txt', 'r') as file:
    lines = file.readlines()

data = {}

for line in lines:
    consumers, time = map(int, line.strip().split())
    if consumers not in data:
        data[consumers] = []
    data[consumers].append(time)

consumers_list = sorted(data.keys())
avg_times = [sum(data[consumers]) / len(data[consumers]) for consumers in consumers_list]

plt.plot(consumers_list, avg_times, marker='o', linestyle='-', color='b')

plt.title('Зависимость времени работы от числа consumer-ов')
plt.xlabel('Число consumer-ов')
plt.ylabel('Среднее время работы, сек.')

plt.grid(True)

plt.savefig('plot.png')
print("График сохранен в файл 'plot.png'")