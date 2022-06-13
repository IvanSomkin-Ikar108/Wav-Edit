## Задание на практику 3 курса по цифровой обработке сигналов
### Практическое задание «Звуковые эффекты»
#### Цели и особенности выполнения

* Получение навыков разработки ПО
* Приобретение первичных знаний о цифровой обработке сигналов

#### Описание

Разработать приложение, работающее со звуковыми файлами WAV, накладывающее различные эффекты и фильтры.

Программа должна обеспечивать возможность:

* Получение данных из заголовка файла и вывод на экран значения параметров:
  * количество каналов,
  * частоту дискретизации,
  * глубину (8, 16, 24, 32 бита),
  * скорость потока (бит/с),
  * длительность записи.
* Обработку файла
  * Обрезка по времени
    * Указывается начальная и конечная временная точка (в единицах времени).
    * Запись обрезается таким образом, чтобы остался только указанный фрагмент записи.
  * Наложение эффекта затухания громкости (уровня).
    * Указывается начальная и конечная временная точка (в единицах времени), и желаемый уровень громкости в процентах в конечной точке (вещественное число).
    * Громкость (уровень) записи начиная от начальной точки должен постепенно уменьшаться, чтобы достигнуть указанного уровня в конечной точке.
  * Наложение эффекта реверберации
    * Указывается два параметра: задержка по времени delay (в единицах времени) и коэффициент затухания decay (вещественное число от 0 до 1)
    * Сигнал обрабатывается в один проход по, например, такой формуле
      * signal[i+delay] += signal[i] * decay;

Способ задания параметров и их значения по умолчанию – по усмотрению разработчика.

В случае повышенного интереса также можно дополнить возможности программы:

* отдельные параметры реверберации по каждому каналу;
* множественные эффекты реверберации;
* расчет параметров реверберации по заданной конфигурации помещения;
* наложение других звуковых эффектов;
* фильтрация сигнала, эквалайзер.

#### Требования

Программа может быть выполнена на любом языке программирования и иметь интерфейс командой строки. По возможности программа должна компилироваться под различные ОС (Windows, Linux). Исходные тексты программы желательно размещать в открытых репозиториях, например, на github.

#### Ссылки на материалы

* Описание формата WAV
  * [https://en.wikipedia.org/wiki/WAV](https://en.wikipedia.org/wiki/WAV)
  * [https://audiocoding.ru/articles/2008-05-22-wav-file-structure](https://audiocoding.ru/articles/2008-05-22-wav-file-structure)
  * [http://soundfile.sapp.org/doc/WaveFormat](http://soundfile.sapp.org/doc/WaveFormat)
* Описание эффекта реверберации
  * [https://en.wikipedia.org/wiki/Reverberation](https://en.wikipedia.org/wiki/Reverberation)
  * [https://ru.wikipedia.org/wiki/Реверберация](https://ru.wikipedia.org/wiki/Реверберация)
  * [http://wikisound.org/Реверберация](http://wikisound.org/Реверберация)
* Вводная статья по цифровому звуку
  * [https://habr.com/ru/post/275613](https://habr.com/ru/post/275613)

#### Сборка с помощью CMake для Linux

Убедитесь, что у вас установлены утилиты git, cmake, make, gcc:

Для Debian / Ubuntu: sudo apt-get install git cmake make gcc  
Для CentOS: sudo yum install git cmake make gcc  
Для Fedora: sudo dnf install git cmake make gcc

1. git clone https://gitflic.ru/project/shurup239/simple-dsp-a.git
2. cd simple-dsp-a
3. mkdir build
4. cd build
5. Для Release сборки: cmake --DCMAKE_BUILD_TYPE=Release ../  Для Debug сборки: cmake --DCMAKE_BUILD_TYPE=Debug ../
6. make
