# TheGolombCode
При p = 0.5
Неоптимальный параметр: m = 7
Теоритическая энтропия = 2
Практическая энтропия = 4

Оптимальный параметр m вычисляется по формуле: 1 - log(1+p)/log(p)
При p = 0.3; 0.5; 0.6 оптимальным m будет 1 
При p = 0.7 m = 2
При p = 0.8 m = 3
При p = 0.9 m = 7
Тогда при p = 0.5   m = 1
Теоритическая энтропия = 1.99
Практическая энтропия = 2.99


-------------------------------------------
В файле "Сравнение распределений (р=0.5).txt" изображены графики теоритического распределения и распределения, полученного на выходе, реализованного источника. Видно, что они совпадают. Для получения данного изображения необходимо воспользоваться кодом, написанном на языке matlab и представленном в файле "entr.m". Перед запуском необходимо изменить значение вероятности, на то, которое вводилось в основной программе.
