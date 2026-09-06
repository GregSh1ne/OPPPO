from dataclasses import dataclass


@dataclass
class Book:
    title: str
    author: str
    genre: str
    copies: int


# Набор готовых тестовых данных
TEST_BOOKS = [
    Book(
        title="Мастер и Маргарита",
        author="Михаил Булгаков",
        genre="Роман / Мистика",
        copies=5,
    ),
    Book(
        title="1984",
        author="Джордж Оруэлл",
        genre="Антиутопия",
        copies=12,
    ),
    Book(
        title="Преступление и наказание",
        author="Фёдор Достоевский",
        genre="Классическая проза",
        copies=7,
    ),
    Book(
        title="Гарри Поттер и философский камень",
        author="Дж. К. Роулинг",
        genre="Фэнтези",
        copies=15,
    ),
    Book(
        title="Этюд в багровых тонах",
        author="Артур Конан Дойл",
        genre="Детектив",
        copies=4,
    ),
]


def get_non_negative_int(prompt: str) -> int:
    """Запрашивает у пользователя целое неотрицательное число."""
    while True:
        raw_input = input(prompt).strip()
        try:
            value = int(raw_input)
            if value < 0:
                print(
                    "Ошибка: значение не может быть отрицательным. Попробуйте снова."
                )
                continue
            return value
        except ValueError:
            print("Ошибка: введено не целое число. Попробуйте снова.")


def get_non_empty_str(prompt: str) -> str:
    """Запрашивает непустую строку."""
    while True:
        value = input(prompt).strip()
        if value:
            return value
        print("Ошибка: поле не может быть пустым.")


def display_catalog(catalog: list[Book]):
    """Выводит список книг и итоговое количество экземпляров."""
    if not catalog:
        print("\n" + "=" * 55)
        print("Каталог пуст (введено 0 книг).")
        print("Общее количество книг в библиотеке: 0")
        print("=" * 55)
        return

    total_copies = sum(book.copies for book in catalog)

    print("\n" + "=" * 55)
    print("СПИСОК ВСЕХ КНИГ В КАТАЛОГЕ")
    print("=" * 55)
    for idx, book in enumerate(catalog, start=1):
        print(
            f"{idx}. «{book.title}» | Автор: {book.author} | Жанр: {book.genre} | Экземпляров: {book.copies}"
        )

    print("-" * 55)
    print(f"Всего наименований: {len(catalog)}")
    print(f"Общее количество экземпляров в библиотеке: {total_copies}")
    print("=" * 55)


def manual_input() -> list[Book]:
    """Ручной ввод книг пользователем с валидацией."""
    n = get_non_negative_int(
        "\nВведите количество наименований книг для добавления (N): "
    )
    if n == 0:
        print("Введено N = 0. Новые книги не были добавлены.")
        return []

    new_books: list[Book] = []
    for i in range(1, n + 1):
        print(f"\n--- Ввод данных для книги #{i} ---")
        title = get_non_empty_str("Название книги: ")
        author = get_non_empty_str("Автор: ")
        genre = get_non_empty_str("Жанр: ")
        copies = get_non_negative_int("Количество экземпляров: ")
        new_books.append(
            Book(title=title, author=author, genre=genre, copies=copies)
        )

    return new_books


def main():
    catalog: list[Book] = []

    while True:
        print("\n" + "#" * 45)
        print("=== Управление библиотечным каталогом ===")
        print("#" * 45)
        print("1 — Загрузить тестовые данные (5 книг)")
        print("2 — Ввести книги вручную (N книг)")
        print("3 — Показать текущий каталог и общее количество")
        print("4 — Очистить каталог (проверка крайнего случая: 0 книг)")
        print("0 — Выйти из приложения")

        choice = input("\nВыберите действие (0-4): ").strip()

        if choice == "1":
            # Загружаем копию готового набора данных
            catalog = [
                Book(b.title, b.author, b.genre, b.copies) for b in TEST_BOOKS
            ]
            print("\n[✓] Тестовые данные успешно загружены в каталог!")
            display_catalog(catalog)

        elif choice == "2":
            new_books = manual_input()
            if new_books:
                catalog.extend(new_books)
                print(f"\n[✓] Успешно добавлено книг: {len(new_books)}.")
            display_catalog(catalog)

        elif choice == "3":
            display_catalog(catalog)

        elif choice == "4":
            catalog.clear()
            print("\n[✓] Каталог успешно очищен.")
            display_catalog(catalog)

        elif choice == "0":
            print("\nЗавершение работы программы. До свидания!")
            break

        else:
            print(
                "\n[!] Некорректный выбор. Пожалуйста, введите цифру от 0 до 4."
            )


if __name__ == "__main__":
    main()