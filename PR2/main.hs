module Main where
import System.IO
import System.Directory (doesFileExist)
import Data.Char (isSpace, toLower)
import Data.List (partition)
import Text.Read (readMaybe)
import Text.Printf (printf)

-- 1. Алгебраический тип данных
data MusicalWork
  = Song     { title :: String, duration :: Double, artist   :: String }
  | Symphony { title :: String, duration :: Double, composer :: String }
  deriving (Show, Eq)

-- Вспомогательная функция приведения строки к нижнему регистру
toLowerStr :: String -> String
toLowerStr = map toLower

-- Парсинг времени "М:СС" или "ММ:СС" с проверкой секунд (< 60) в дробные минуты
parseDuration :: String -> Maybe Double
parseDuration str =
  case break (== ':') str of
    (mStr, ':':sStr) -> do
      mins <- readMaybe mStr
      secs <- readMaybe sStr
      if mins >= 0 && secs >= (0 :: Int) && secs < 60
        then Just (fromIntegral mins + fromIntegral secs / 60.0)
        else Nothing
    _ -> do
      val <- readMaybe str
      if val >= (0.0 :: Double) then Just val else Nothing

-- Форматирование длительности в "М:СС"
formatDuration :: Double -> String
formatDuration d =
  let totalSecs = round (d * 60.0) :: Int
      mins = totalSecs `div` 60
      secs = totalSecs `mod` 60
  in printf "%d:%02d" mins secs

-- Вывод отдельного произведения в строку с форматированием колонок
formatWork :: Int -> MusicalWork -> String
formatWork idx work =
  case work of
    Song t d a ->
      printf "%2d. %-12s | Название: %-24s | Длительность: %-6s (%0.2f мин.) | Исполнитель: %s"
        idx ("[Песня]" :: String) ("\"" ++ t ++ "\"") (formatDuration d) d a
    Symphony t d c ->
      printf "%2d. %-12s | Название: %-24s | Длительность: %-6s (%0.2f мин.) | Композитор:  %s"
        idx ("[Симфония]" :: String) ("\"" ++ t ++ "\"") (formatDuration d) d c

printCatalog :: [MusicalWork] -> IO ()
printCatalog works = do
  putStrLn "\n================ ТЕКУЩЕЕ СОДЕРЖИМОЕ КАТАЛОГА ================"
  if null works
    then putStrLn "  (Каталог пуст)"
    else mapM_ (putStrLn . uncurry formatWork) (zip [1..] works)
  printf "Всего объектов в каталоге: %d\n" (length works)
  putStrLn "============================================================\n"

-- Проверка совпадения произведения с заданным предикатом (REM)
matchesCondition :: String -> String -> String -> MusicalWork -> Bool
matchesCondition field op val work =
  let f = toLowerStr field
      vLower = toLowerStr val
  in case f of
    "duration" ->
      case parseDuration val of
        Just target ->
          let d = getDuration work
              eps = 1.0 / 60.0 -- точность до 1 секунды
          in case op of
            "==" -> abs (d - target) < eps
            "!=" -> abs (d - target) >= eps
            ">"  -> d > target
            "<"  -> d < target
            ">=" -> d >= target
            "<=" -> d <= target
            _    -> False
        Nothing -> False
    "title" ->
      compareStr (toLowerStr (getTitle work)) op vLower
    "type" ->
      compareStr (toLowerStr (getType work)) op vLower
    "artist" ->
      case work of
        Song _ _ a -> compareStr (toLowerStr a) op vLower
        _          -> False
    "composer" ->
      case work of
        Symphony _ _ c -> compareStr (toLowerStr c) op vLower
        _              -> False
    _ -> False
  where
    getTitle (Song t _ _)     = t
    getTitle (Symphony t _ _) = t
    getDuration (Song _ d _)     = d
    getDuration (Symphony _ d _) = d
    getType (Song {})     = "Song"
    getType (Symphony {}) = "Symphony"

    compareStr a "==" b = a == b
    compareStr a "!=" b = a /= b
    compareStr _ _    _ = False

-- Корректно извлекает токены, включая аргументы в кавычках ""
tokenize :: String -> [String]
tokenize [] = []
tokenize (c:cs)
  | isSpace c = tokenize cs
  | c == '"'  = let (word, rest) = span (/= '"') cs
                in word : tokenize (drop 1 rest)
  | otherwise = let (word, rest) = break (\x -> isSpace x || x == '"') (c:cs)
                in word : tokenize rest

-- Хвостовая рекурсия обработки команд файла
runCommands :: [MusicalWork] -> [(Int, String)] -> IO ()
runCommands _ [] = return ()
runCommands catalog ((lineNum, rawLine):rest) =
  case dropWhile isSpace rawLine of
    []        -> runCommands catalog rest
    ('#':_)   -> runCommands catalog rest
    line      -> case tokenize line of
      ("ADD" : kind : t : durStr : person : _) ->
        case parseDuration durStr of
          Nothing -> do
            printf "Строка %d: некорректный формат длительности '%s' (секунды должны быть от 0 до 59)\n" lineNum durStr
            runCommands catalog rest
          Just dur ->
            case toLowerStr kind of
              "song" -> do
                let item = Song t dur person
                printf "[ADD] Добавлена песня: \"%s\" (%s)\n" t (formatDuration dur)
                runCommands (catalog ++ [item]) rest
              "symphony" -> do
                let item = Symphony t dur person
                printf "[ADD] Добавлена симфония: \"%s\" (%s)\n" t (formatDuration dur)
                runCommands (catalog ++ [item]) rest
              _ -> do
                printf "Строка %d: неизвестный тип: %s\n" lineNum kind
                runCommands catalog rest

      ("REM" : field : op : val : _) -> do
        let (kept, removed) = partition (not . matchesCondition field op val) catalog
        printf "[REM] Удалено по условию (%s %s \"%s\"): %d шт.\n" field op val (length removed)
        runCommands kept rest

      ["PRINT"] -> do
        printCatalog catalog
        runCommands catalog rest

      (cmd : _) -> do
        printf "Строка %d: неизвестная команда: %s\n" lineNum cmd
        runCommands catalog rest

      [] -> runCommands catalog rest

main :: IO ()
main = do
  hSetEncoding stdout utf8
  hSetEncoding stderr utf8

  let filename = "commands.txt"
  putStrLn $ "Запуск обработки команд из файла '" ++ filename ++ "'..."

  exists <- doesFileExist filename
  if not exists
    then putStrLn $ "Ошибка: не удалось открыть файл " ++ filename
    else do
      -- Открытие и чтение файла строго в кодировке UTF-8
      withFile filename ReadMode $ \h -> do
        hSetEncoding h utf8
        content <- hGetContents h
        let indexedLines = zip [1..] (lines content)
        runCommands [] indexedLines