# search_system
Эта программа была создана в рамках обучения на курсе по языку программирования C++

В данной задаче необходимо поработать с классом SearchServer, который позволяет выполнять поиск в базе документов.

Метод AddQueriesStream выполняет собственно поиск. Он принимает входной поток поисковых запросов и выходной поток для записи результатов поиска. При этом:
Один документ — это одна строка входного потока;
документы состоят из слов, разделённых одним или несколькими пробелами;
слова состоят из строчных латинских букв.

Результатом обработки поискового запроса является набор из максимум пяти наиболее релевантных документов (документы, в которых нет ни одного слова из запроса, в качестве результата не выводятся).

Метод UpdateDocumentBase заменяет текущую базу документов на новую, которая содержится в потоке document_input.

Метод AddQueriesStream должен быть готов к тому, что параллельно с ним будет выполняться метод UpdateDocumentBase и наоборот.

Метод AddQueriesStream не обязан обрабатывать все запросы с той версией базы документов, которая была актуальна во время его запуска. То есть, если во время работы метода AddQueriesStream база документов была обновлена, он может использовать новую версию базы для обработки оставшихся запросов.


Ограничения на вход:

document_input содержит не более 50000 документов
каждый документ содержит не более 1000 слов
общее число различных слов во всех документах не превосходит 15000
максимальная длина одного слова — 100 символов, слова состоят из строчных латинских букв и разделены одним или несколькими пробелами
query_input содержит не более 500 000 запросов, каждый запрос содержит от 1 до 10 слов.


Изначально были даны некоторые исходники, которые работали верно, но медленно. Основной проблемой являлось сделать это достаточно быстрым.

Например, тест со следующими данными: 6 streams of 11000 documents with at most 50 words each, 6 streams of 12000 queries), - должен был отработать максимум за 909мс.

Помимо кода задачи, в решении так же находятся разработанные в ходе курса небольшие тестовый драйвер, профилировщик и класс для синхронизации обращения к общим данным.
