-- Лекция 07 Аппликативные функторы. Аппликативные парсеры

{-
В качестве основных и дополнительных материалов посмотрите:
Москвин - лекция 8 Аппликативные функторы, лекция 9 Использование аппликативных функторов
Программируй на Haskell - Модуль 5
-}



import Data.Char (isLower, isUpper, digitToInt, isDigit)
import Control.Applicative (Alternative(..), optional)--, ZipList(..))
--import Control.Applicative
import Data.Monoid (Alt(..),Ap(..),Sum(..))
-- import Data.Functor.Identity (Identity(..))
import Data.Coerce (coerce)

{-
($)   ::   (a -> b) ->    a ->    b
map   ::   (a -> b) -> [] a -> [] b
(<$>) ::   (a -> b) -> f  a -> f  b
(<*>) :: f (a -> b) -> f  a -> f  b

-- (+1) $ 2
-- map (+1) [1,2,3]
-- fmap (+1) (Just 2) -- (+1) <$> Just 2
-- Just (+1) <*> Just 2 -- pure (+1) <*> Just 2

-- a = [(+1), (*3), (^2)]
-- a <*> [3]
-- Just (a <*>) <*> Just [3]

-}


{-
Как поднять значение в контекст?
(т.е. имея значение положить его в контейнер)

pure :: a -> f a

instance _ Maybe where
    pure x = Just x

instance _ [] where
    pure x = [x]

instance _ (Either e) where
    pure x = Right x

instance _ ((->) e) where
    pure x = \_ -> x -- const

instance _ Tree where
    pure x = Leaf x

instance _ ((,) s) where
    pure x = (mempty, x)

Закон:
fmap g . pure = pure . g
-- a -> pure -> f a -> fmap g -> f b
-- a -> g    -> b   -> pure   -> f b

-}

-----------------------------------------------------------------------------------------------------

-- import Control.Applicative

{-
Аппликативные функторы

-- (+1) <$> Just 2
-- :t (Just (+))
-- :t ((+) <$> Just 2)
-- ((+) <$> Just 2 <$> Just 3)

fmap :: (a -> b) -> f a -> f b

fmap2 :: (a -> b -> c) -> f a -> f b -> f c
fmap2 g as bs = fmap g as `ap` bs
                fmap g as :: f (b -> c)
                          `ap` :: f (b -> c) -> f b -> f c

fmap3 :: (a -> b -> c -> d) -> f a -> f b -> f c -> f d
fmap3 g as bs cs = (fmap g as `ap` bs) `ap` cs
...
`ap` :: f (b -> c) -> f b -> f c -- стрелк в контексте

Универсальную ap невозможно получить для произвольного функтора

class Functor f => Applicative f where
    pure :: a -> f a
    (<*>) :: f (a -> b) -> f a -> f b

-- (+) <$> Just 2 <*> Just 3
-- pure (+) <$> Just 2 <*> Just 3
-- pure (+) <*> Just 2 <*> Just 3


instance Applicative Maybe where
    pure = Just
    Nothing <*> _ = Nothing
    (Just g) <*> x = fmap g x

-- Just (+2) <*> Just 5
-- Just (+2) <*> Nothing
-- Just (+) <*> Just 2 <*> Just 5


Закон связи Applicative и Functor:
fmap g xs ≡ pure g <*> xs -- pure должен быть «безэффектным»


Законы Applicative:

Identity: pure id <*> v ≡ v
Homomorphism: pure g <*> pure x ≡ pure (g x)
Interchange: u <*> pure x ≡ pure ($ x) <*> u
Composition: pure (.) <*> u <*> v <*> x ≡ u <*> (v <*> x)


Списки как аппликативные функторы:
fs = [(2*),(3+),(4-)]
xs = [1,2]
-- fs <*> xs

Список — контекст, задающий множественные результаты недетерминированного вычисления:
fs <*> xs → [(2*)1,(2*)2,(3+)1,(3+)2,(4-)1,(4-)2] → [2,4,4,5,3,2]
Список — это коллекция упорядоченных элементов:
fs <*> xs → [(2*)1,(3+)2] → [2,5]

Стандартный представитель списков для аппликатива:
instance Applicative [] where
    pure x = [x]
    gs <*> xs = [ g x | g <- gs, x <- xs ]
-}
-- Представитель списка как коллекция упорядоченных элементов:
newtype ZipList a = ZipList { getZipList :: [a] }

instance Functor ZipList where
    fmap f (ZipList xs) = ZipList (map f xs)

instance Applicative ZipList where
    pure x = ZipList (repeat x)
    ZipList gs <*> ZipList xs = ZipList (zipWith ($) gs xs)

{-
fs = [(2*),(3+),(4-)]
xs = [1,2]
getZipList $ ZipList fs <*> ZipList xs


instance Monoid s => Applicative ((,) s) where
    pure x = (mempty, x)
    (u, f) <*> (v, x) = (u <> v, f x)

-- ("Answer to ",(*)) <*> ("the Ultimate ",6) <*> ("Question",7)

class Functor f => Applicative f where
    {-# MINIMAL pure, ((<*>) | liftA2) #-}
    pure :: a -> f a
    
    (<*>) :: f (a -> b) -> f a -> f b
    (<*>) = liftA2 id
    
    liftA2 :: (a -> b -> c) -> f a -> f b -> f c
    liftA2 g a b = g <$> a <*> b
    
    (*>) :: f a -> f b -> f b
    a1 *> a2 = (id <$ a1) <*> a2
    
    (<*) :: f a -> f b -> f a
    (<*) = liftA2 const
-}
-- [1,2,3] <* "AS"
-- [1,2,3] *> "AS"


{-
Синтаксический анализатор (парсер) - преобразует выходные данные в структурированный формат для последующего использования или анализа
                                    если входные данные не удовлетворяют грамматике, то возвращается ошибка

Например:
    Транслятор (компилятор, интерпретатор) - текст -> команды
    Разбор баз данных (CSV, XML, ...)
    Разбор фалов 3D объектов
    Анализ текстов

Возможыне парсеры:

type Parser a = String -> a
                Строка -> Структура

type Parser a = String -> (String, a)
    Комбинирование парсеров: (Остаток строки, Структура)

type Parser a = String -> Maybe (String, a)
    Обработка простых ошибок

type Parser a = String -> Either String (String, a)
    Обработка сложных ошибок

type Parser a = String -> [(String, a)]
    Обработка неоднозначных грамматик



Абстракция по типу входного потока дял обработки лексем
type Parser tok a = [tok] -> Maybe ([tok], a)

-}

newtype Parser tok a = Parser {runParser :: [tok] -> Maybe ([tok], a) }

-- Простейший парсер строк:
-- парсим значения 'A' в начале строки
charA :: Parser Char Char -- поток данных типа Char и структура данных типа Char
charA = Parser f where -- f - стрелка типа: [tok] -> Maybe ([tok], a)
    f (c:cs) | c == 'A' = Just (cs,c)
    f _ = Nothing
{-
-- runParser charA "ABC"
Запуск парсера через функцию runParser
    В runParser передается парсер charA
        runParser вынимает из charA стрелку: [tok] -> Maybe ([tok], a)
        т.е. возврщается функция из списка токенов [tok] в Maybe ([tok], a)
    В полученную функцию передается строка "ABC"
    Выполняется: f (c:cs) | c == 'A' = Just (cs,c)
        т.к. первый символ 'A' то возвращается сборка для Maybe ([tok], a)
        т.е. Just ("BC",'A')

-- runParser charA "BCD"
Запуск -//-
    Выполняется: f (c:cs) | c == 'A' = Just (cs,c)
        т.к. первый символ 'B' переходим к следующей строке
    Выполняется: f _ = Nothing, возвращается Nothing
-}


-- Функция для конструирования парсеров:
satisfy :: (tok -> Bool) -> Parser tok tok -- (tok -> Bool) -- унарный предикат над токенами
satisfy pr = Parser f where
    f (c:cs) | pr c  = Just (cs,c) -- pr -- проверяет первый символ строки на соответствие условию
    f _= Nothing
{-
Унарные предикаты:
-- :t isUpper
-- :t isLower

-- runParser (satisfy isUpper) "ABC"
-- runParser (satisfy isLower) "ABC"
-}

lower :: Parser Char Char
lower = satisfy isLower
-- runParser lower "asd"

-- обобщение парсера символов:
char :: Char -> Parser Char Char
char c = satisfy (== c)
-- runParser (char 'a') "asd"


-- Парсер символов в числа
{-
:t isDigit
-- runParser (satisfy isDigit) "ABC"
-- runParser (satisfy isDigit) "1BC"
Just ("BC",'1') -- '1' :: Char

Нужно изменить тип: Char -> Int
Для этого можно использовать функтор:
    fmap :: (a    -> b)   -> f             a     -> f             b
            (Char -> Int) -> Just (String, Char) -> Just (String, Int)

:t digitToInt

-}

digit :: Parser Char Int
digit = digitToInt <$> satisfy isDigit

-- представитьль функторов:
instance Functor (Parser tok) where
    fmap :: (a -> b) -> Parser tok a -> Parser tok b
    fmap g (Parser p) = Parser f where -- f :: [tok] -> Maybe ([tok], a)
        f xs = case p xs of -- p из запуска парсера (Parser p)
            Nothing -> Nothing
            Just (cs, c) -> Just (cs, g c)
-- так как Parser - это последовательность композиций трех функторов:
-- (->) [tok], Maybe и (,) [tok], то можно записать через fmap:
-- fmap g (Parser p) = Parser $ (fmap . fmap . fmap) g p
-- или
-- fmap g = Parser . (fmap . fmap . fmap) g . runParser
-- записаь через coerce:
-- fmap :: forall a b . (a -> b) -> Parser tok a -> Parser tok b
-- fmap = coerce ( fmap . fmap . fmap :: (a -> b) -> ([tok] -> Maybe ([tok], a)) -> [tok] -> Maybe ([tok], b) )

-- runParser digit "12AB"
-- runParser digit "AB12"



-- Счепление нескольких парсеров: 

-- Аппликативный парсер
{-
Для сцепления парсеров нужнен предсавитель Applicative:
    pure: парсер, всегда возвращающий заданное значение
    (<*>): получить результаты первого парсера, затем второго, а после этого применить первые ко вторым
Т.е. сцепление нескольких парсеров в один общий парсер производит несколько эффектов, которые реализуют синтаксический разбор
-}

instance Applicative (Parser tok) where
    pure :: a -> Parser tok a
    pure x = Parser $ \s -> Just (s, x) -- безэффектное вычисление - строка не обрабатывается и вместе со значением кладется в упаковку
-- runParser (pure 42) "ABCD"
    (<*>) :: Parser tok (a -> b) -> Parser tok a -> Parser tok b -- первый парсер - стрелка, второй - значение
-- получаем результат первого парсера, затем второго на остатке строки, и применяем первый ко второму
-- неудача хотя бы одного парсера приводит к тотальной неудаче
    Parser u <*> Parser v = Parser f where
        f xs = case u xs of -- f - результирующий парсер запущенный на списке токенов
                    -- u - запускаем первый парсер
            Nothing -> Nothing
            Just (xs', g) -> case v xs' of
                                -- v - запускаем второй парсер (на хвосте xs' от результата первого парсера)
                Nothing -> Nothing
                Just (xs'', x) -> Just (xs'', g x)
-- runParser (pure (,) <*> digit <*> digit) "12AB"
-- runParser ((,) <$> digit <*> digit) "1AB2"
-- runParser ((,) <$> digit <*> charA) "1AB2"

-- пример парсера для строки вида: число произведение число
multiplication :: Parser Char Int
multiplication = (*) <$> digit <* char '*' <*> digit
-- runParser multiplication "2*3"
-- runParser multiplication "g*3"
-- runParser multiplication "12*345"

{-
Парсинг "12*345" через <*> требует только удачных сопоставлений
Как выполнять действия с произвольным числом знаков справа и слева от знака?
-}



-- Класс типов Alternative
{-
Alternative расширяет Applicative дополнительными методами empty и (<|>)
:i Alternative

class Applicative f => Alternative f where
    empty :: f a
    (<|>) :: f a -> f a -> f a -- :t (<*)
-- моноидальная операция с семантикой сложения

-- Представитель Alternative для списков полностью повторяет определение моноида для списка
instance Alternative [] where
    empty :: [a]
    empty = []

    (<|>) :: [a] -> [a] -> [a]
    (<|>) = (++)


-- Представитель моноида для Maybe:
class Monoid a where
    mempty :: a
    mappend :: a -> a -> a
instance Monoid a => Monoid (Maybe a) where -- ограничение моноида на содержимое контейнера
    mempty = Nothing
    Nothing `mappend` m = m
    m `mappend` Nothing = m
    Just m1 `mappend` Just m2 = Just (m1 `mappend` m2)

-- Представитель Alternative для Maybe не повторяет моноид:
instance Alternative Maybe where
    empty :: Maybe a
    empty = Nothing

    (<|>) :: Maybe a -> Maybe a -> Maybe a
    Nothing <|> m = m
    m <|> _ = m -- возвращаем первую не Nothing альтернативу
--  Nothing <|> Just 3 <|> Just 5 <|> Nothing


-- Alternative для списков другой семантики:
instance Alternative ZipList where
    empty :: Maybe a
    empty = ZipList []

    (<|>) :: ZipList a -> ZipList a -> ZipList a
    ZipList xs <|> ZipList ys = ZipList (xs ++ drop (length xs) ys) -- дополняем первый список остатками второго
-- ZipList "abc" <|> ZipList "ABCDEFG"

-}

instance Alternative (Parser tok) where
    empty :: Parser tok a
    empty = Parser $ \_ -> Nothing -- базовый случай - убить вычисление
  
    (<|>) :: Parser tok a -> Parser tok a -> Parser tok a
    Parser u <|> Parser v = Parser f where 
        f xs = case u xs of
            Nothing -> v xs -- если левый парсер ошибка, то переходим к правому
            z -> z
-- runParser  (char 'A' <|> char 'B') "ABC"
-- runParser  (char 'A' <|> char 'B') "BCD"
-- runParser  (char 'A' <|> char 'B') "CDE"

-- Рекурсивный парсер из альтернативы для строки в нижнем регистре
lowers :: Parser Char String
lowers = (:) <$> lower <*> lowers <|> pure ""
        -- (:) - создание строки
                -- lower - парсер
                        -- lowers - рекурсия
                                    -- pure "" - "терминирующая" альтернатива
                                    -- lowers' = (:) <$> lower <*> lowers'
-- runParser lowers "abGHdef"
-- runParser lowers "abdef"
-- runParser lowers "GHabdef"

{-
Обобщение парсеров типа lowers:
    lowers = (:) <$> lower <*> lowers

class Applicative f => Alternative f where
    empty :: f a
    (<|>) :: f a -> f a -> f a

    some, many :: f a -> f [a]
    some v = (:) <$> v <*> many v -- Один и более
    many v = some v <|> pure [] -- Ноль и более

    optional :: Alternative f => f a -> f (Maybe a)
    optional v = Just <$> v <|> pure Nothing

-}

-- runParser (many digit) "42abdef"
-- runParser (some digit) "42abdef"

-- runParser (many digit) "abdef"
-- runParser (some digit) "abdef"

-- runParser (optional digit) "42abdef"
-- runParser (optional digit) "abdef"



-- Класс типов Traversable
{-
Аппликативный дистрибьютор списка
Как поменять список и обертку? (т.е. вынуть аппликативный эффект наружу)
[f a] -> f [a]
    f :: * -> *
    [] :: * -> *
-}

dist :: Applicative f => [f a] -> f [a]
dist [] = pure [] -- обертка пустого списка в аппликативный функтор
dist (ax:axs) = pure (:) <*> ax <*> dist axs -- рекурсивный разбор списка и применение содержания списка
-- все эффекты сохраняются, но меняются местами
-- dist [Just 3,Just 5]
-- dist [Just 3,Nothing]
-- getZipList $ dist $ map ZipList [[1,2], [3,4], [5,6]]
-- dist [[1,2], [3,4], [5,6]]

{-
Можно обобщить список до t :: * -> *
f :: * -> *
t :: * -> *
t (f a) -> f (t a)

:i Traversable

class (Functor t, Foldable t) => Traversable t where
    sequenceA :: Applicative f => t (f a) -> f (t a)
    sequenceA = traverse id
    -- sequenceA - определяет правило коммутации функтора t с произвольным аппликативным функтором f
    -- структура внешнего контейнера t сохраняется, а аппликативные эффекты внутренних f объединяются в результирующем f

    traverse :: Applicative f => (a -> f b) -> t a -> f (t b)
    traverse g = sequenceA . fmap g
                            fmap g :: t (f b)
                sequenceA                     -> f (t b)
    -- поднимает стрелку (a -> f b) в вычислительный контекст
    -- проходим по структуре t a, последовательно применяя функцию к элементам типа a и собирая структуру из результатов типа b
    -- эффекты коллекционируются (fmap с эффектами)

-- sequenceA = 
-- traverse    :: (a -> f b) -> t a -> f (t b)
            id :: (a -> f b)
            a = f b
            id :: f b -> f b


-- представители для Maybe и списка:
instance Traversable Maybe where
    traverse :: Applicative f => (a -> f b) -> Maybe a -> f (Maybe b)
    traverse _ Nothing = pure Nothing
    traverse g (Just x) = Just <$> g x

instance Traversable [] where
    traverse :: Applicative f => (a -> f b) -> [a] -> f [b]
    traverse _ [] = pure []
    traverse g (x:xs) = (:) <$> g x <*> traverse g xs

-- Представители Traversable аналогичны по структуре представителям Functor

-- traverse (\x -> [x+10, x+100]) (Just 7)
-- traverse (\x -> [x+10, x+100]) [7,8,9]
-- traverse (\x -> [x+10, x+100, x+1000]) [7,8]



Сравнение реализаций Traversable и Functor

instance Traversable Maybe where
    traverse _ Nothing  = pure Nothing
    traverse g (Just x) = pure Just <*> g x
instance Functor Maybe where
    fmap     _ Nothing  =      Nothing
    fmap     g (Just x) =      Just    (g x)

instance Traversable [] where
    traverse _ []     = pure []
    traverse g (x:xs) = pure (:) <*> g x <*> traverse g xs
instance Functor [] where
    fmap _ []         =      []
    fmap g (x:xs)     =      (:)    (g x)   (fmap     g xs)

-- Traversable — это Functor: имея traverse мы можем универсальным образом реализовать fmap, удовлетворяющий законам функтора
-- Можно реализовать содержательно Traversable, а Functor и Foldable получить реализациями по умолчанию



Законы Traversable (наследуют законам функторов):

(1) identity:
traverse Identity ≡ Identity
-}
newtype Identity a = Identity { runIdentity :: a } deriving (Show) -- Id :: a -> Id a
instance Functor Identity where
    fmap g (Identity x) = Identity (g x)
instance Applicative Identity where
    pure = Identity
    Identity g <*> v = fmap g v
-- traverse Identity [1,2,3]
-- runIdentity (traverse Identity [1,2,3])
-- runIdentity (traverse Identity (Just 1))

{-
(2) composition
traverse (Compose . fmap f2 . f1) ≡ Compose . fmap (traverse f2) . traverse f1
                              f1 :: a -> g1 b
                         f2 :: b -> g2 c
    обе части :: t a -> Compose g2 g1 (t с)
Композиция (a -> g1 b) и (b -> g2 c) эквивалентна (a -> g1 (g2 c))

(3) naturality
h . traverse f ≡ traverse (h . f)

h :: (Applicative f, Applicative g) => f b -> g b - функция удовлетворяющая требованиям:
    (1) h (pure x) = pure x
    (2) h (x <*> y) = h x <*> h y
f :: a -> f b, обе части :: t a -> g (t b)

Практический смысл Traversable:
    - траверсы не пропускают элементов
    - траверсы посещают элементы не более одного раза
    - traverse pure дает pure
    - траверсы не изменяют исходную структуру — она либо сохраняется, либо полностью исчезает

-- traverse Just [1,2,3]
-- traverse (const Nothing) [1,2,3]

-}

{-
Обертка Alt
Alt превращает любой Alternative функтор в Semigroup и Monoid, используя операцию <|> из Alternative

newtype Alt f a = Alt {getAlt :: f a}
instance Alternative f => Semigroup (Alt f a) where
    (<>) = coerce ((<|>) :: f a -> f a -> f a)
    stimes = stimesMonoid
instance Alternative f => Monoid (Alt f a) where
    mempty = Alt empty

-- Alt "Abc" <> Alt "De"
-- Alt Nothing <> Alt (Just 1) <> Alt (Just 2)

-}

{-
Обертка Ap
Ap позволяет "поднять" операцию моноида (<>, mempty) внутрь аппликативного функтора (Applicative f), используя liftA2.

newtype Ap f a = Ap { getAp :: f a }
instance (Applicative f,Semigroup a) => Semigroup (Ap f a) where
    (Ap x) <> (Ap y) = Ap $ liftA2 (<>) x y
instance (Applicative f, Monoid a) => Monoid (Ap f a) where
    mempty = Ap $ pure mempty

-- apLstS1 = Ap $ Sum <$> [1,2,3]
-- apLstS2 = Ap $ Sum <$> [10,20]
-- getAp $ getSum <$> (apLstS1 <> apLstS2)

-}

