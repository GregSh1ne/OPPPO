from dataclasses import asdict, dataclass
import json
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_FILE = os.path.join(SCRIPT_DIR, "library_catalog.json")


@dataclass
class Book:
    title: str
    author: str
    genre: str
    copies: int


def normalize(text: str) -> str:
    """Удаляет лишние пробелы по краям и приводит строку к нижнему регистру."""
    return text.strip().lower()


def find_book(catalog: list[Book], title: str, author: str) -> Book | None:
    """Ищет книгу в каталоге по названию и автору без учёта регистра."""
    target_title = normalize(title)
    target_author = normalize(author)

    for book in catalog:
        if (
            normalize(book.title) == target_title
            and normalize(book.author) == target_author
        ):
            return book
    return None


def save_catalog_to_file(
    catalog: list[Book], filename: str = DATA_FILE
) -> bool:
    """Сохраняет текущий каталог книг в JSON-файл."""
    try:
        data = [asdict(book) for book in catalog]
        with open(filename, "w", encoding="utf-8") as file:
            json.dump(data, file, ensure_ascii=False, indent=4)
        print(f"Каталог успешно сохранён в файл '{filename}'.")
        return True
    except OSError as error:
        print(f"Ошибка при записи в файл '{filename}': {error}")
        return False


def load_catalog_from_file(filename: str = DATA_FILE) -> list[Book]:
    """Загружает список книг из JSON-файла с объединением дубликатов."""
    if not os.path.exists(filename):
        return []

    try:
        with open(filename, "r", encoding="utf-8") as file:
            raw_data = json.load(file)

        loaded_catalog: list[Book] = []
        for item in raw_data:
            title = str(item.get("title", "")).strip()
            author = str(item.get("author", "")).strip()
            genre = str(item.get("genre", "")).strip()
            copies = int(item.get("copies", 0))

            existing = find_book(loaded_catalog, title, author)
            if existing:
                existing.copies += copies
            else:
                loaded_catalog.append(
                    Book(title=title, author=author, genre=genre, copies=copies)
                )

        return loaded_catalog
    except (json.JSONDecodeError, OSError, ValueError) as error:
        print(f"Ошибка чтения файла '{filename}': {error}")
        return []


def get_non_negative_int(prompt: str) -> int:
    """Запрашивает целое неотрицательное число."""
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
    """Выводит список книг и общее количество экземпляров."""
    if not catalog:
        print("\nКаталог пуст (введено 0 книг).")
        print("Общее количество книг в библиотеке: 0")
        return

    total_copies = sum(book.copies for book in catalog)

    print("\nСПИСОК ВСЕХ КНИГ В КАТАЛОГЕ:")
    for idx, book in enumerate(catalog, start=1):
        print(
            f"{idx}. «{book.title}» | Автор: {book.author} | Жанр: {book.genre} | Экземпляров: {book.copies}"
        )

    print(f"\nВсего уникальных наименований (N): {len(catalog)}")
    print(f"Общее количество экземпляров в библиотеке: {total_copies}")


def add_books_to_catalog(catalog: list[Book]):
    """Организует ввод N книг и объединяет совпадения с существующими."""
    n = get_non_negative_int(
        "\nВведите количество наименований книг для добавления (N): "
    )
    if n == 0:
        print("Введено N = 0. Каталог оставлен без изменений.")
        return

    added_new = 0
    merged_existing = 0

    for i in range(1, n + 1):
        print(f"\nВвод данных для книги {i} из {n}:")
        title = get_non_empty_str("Название книги: ")
        author = get_non_empty_str("Автор: ")
        genre = get_non_empty_str("Жанр: ")
        copies = get_non_negative_int("Количество экземпляров: ")

        existing_book = find_book(catalog, title, author)
        if existing_book:
            existing_book.copies += copies
            merged_existing += 1
            print(
                f"Книга «{existing_book.title}» ({existing_book.author}) уже есть в каталоге. "
                f"Экземпляры объединены: +{copies} (всего: {existing_book.copies})."
            )
        else:
            catalog.append(
                Book(title=title, author=author, genre=genre, copies=copies)
            )
            added_new += 1
            print(f"Книга «{title}» успешно добавлена в каталог.")

    print(
        f"\nИтог добавления: новых наименований — {added_new}, обновлено существующих — {merged_existing}."
    )


def main():
    catalog: list[Book] = load_catalog_from_file(DATA_FILE)

    print("Управление библиотечным каталогом")
    if catalog:
        print(
            f"Файл '{DATA_FILE}' найден: автоматически загружено {len(catalog)} наименований."
        )
    else:
        print(
            f"Файл данных не найден или пуст. Каталог инициализирован с 0 книг."
        )

    while True:
        print("\nМеню приложения:")
        print("1 — Показать текущий каталог и общее количество")
        print("2 — Добавить книги (N наименований)")
        print("3 — Сохранить каталог в файл")
        print("4 — Перезагрузить каталог из файла")
        print("5 — Очистить каталог (крайний случай: 0 книг)")
        print("0 — Выйти из приложения")

        choice = input("\nВыберите действие (0-5): ").strip()

        if choice == "1":
            display_catalog(catalog)

        elif choice == "2":
            add_books_to_catalog(catalog)
            display_catalog(catalog)

        elif choice == "3":
            save_catalog_to_file(catalog, DATA_FILE)

        elif choice == "4":
            catalog = load_catalog_from_file(DATA_FILE)
            print(f"Каталог перезагружен. Текущих записей: {len(catalog)}.")
            display_catalog(catalog)

        elif choice == "5":
            catalog.clear()
            print("Каталог в оперативной памяти очищен.")
            display_catalog(catalog)

        elif choice == "0":
            save_prompt = (
                input("Сохранить текущие данные в файл перед выходом? (y/n): ")
                .strip()
                .lower()
            )
            if save_prompt in ("y", "yes", "д", "да"):
                save_catalog_to_file(catalog, DATA_FILE)
            print("Работа завершена.")
            break

        else:
            print("Некорректный ввод. Введите число от 0 до 5.")


if __name__ == "__main__":
    main()