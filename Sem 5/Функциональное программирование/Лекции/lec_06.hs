-- Лекция 06 Свертки. Полугруппы. Моноиды. Функторы. Аппликативные функторы

{-
В качестве основных и дополнительных материалов посмотрите:
Москвин - лекция 7 Свертки, лекция 8 Аппликативные функторы
Программируй на Haskell - Модуль 3, Модуль 5
-}

-----------------------------------------------------------------------------------------------------
{- Свертки

-- Стандартные функции для списков:

sum :: [Integer] -> Integer
sum [] = 0
sum (x:xs) = x + sum xs

product :: [Integer] -> Integer
product [] = 1
product (x:xs) = x * product xs

concat :: [[a]] -> [a]
concat [] = []
concat (x:xs) = x ++ concat xs

-- Обобщение:

-- Правая свертка:
foldr :: (a -> b -> b) -> b -> [a] -> b
foldr f initial [] = initial
foldr f initial (x:xs) = f x (foldr f initial xs)

-- foldr (+) 0 [1,2,3]

foldr (+) 0 [1,2,3] => 1 + foldr (+) 0 [2,3]
                    => 1 + (2 + foldr (+) [3])
                    => 1 + (2 + (3 + foldr (+) 0 []))
                    => 1 + (2 + (3 + 0))
                    => 1 + (2 + 3) => 1 + 5 => 6

e1 : (e2 : (e3 : [])) -> e1 `f` (e2 `f` (e3 `f` initial))

-- foldr (*) 1 [1,2,3]
-- foldr (++) [] ["asd", "fgh"]
-- foldr (\n b -> odd n && b) True [1,2,3]

-}

data InfNumber a = MinusInfinity
                 | Number a
                 | PlusInfinity
                 deriving Show

infMax MinusInfinity x = x
infMax x MinusInfinity = x
infMax PlusInfinity _ = PlusInfinity
infMax _ PlusInfinity = PlusInfinity
infMax (Number a) (Number b) = Number (max a b)
-- foldr infMax MinusInfinity $ map Number [1,2,3]
-- foldr infMax MinusInfinity $ ((map Number [1,2,3]) ++ [PlusInfinity])
-- foldr (\x y -> infMax (Number x) y) MinusInfinity [1,2,3]

{-
-- Левая свертка

foldl :: (b -> a -> b) -> b -> [a] -> b
foldl f initial [] = initial
foldl f initial (x:xs) = foldl f (f initial x) xs

-- foldl (+) 0 [1,2,3]
{-
foldl (+) 0 [1,2,3] => foldl (+) (0 + 1) [2,3]
                    => foldl (+) ((0 + 1) + 2) [3]
                    => foldl (+) (((0 + 1) + 2) + 3) []
                    => ((0 + 1) + 2) + 3 => (1 + 2) + 3 => 3 + 3 => 6
-}
-- foldl (\x y -> infMax x (Number y)) MinusInfinity [1,2,3]

e1 : (e2 : (e3 : [])) -> ((initial `f` e1) `f` e2) `f` e3)

Появляются отложенные вычисления (thunk)

Строгая версия:

foldl :: (b -> a -> b) -> b -> [a] -> b
foldl f initial [] = initial
foldl f initial (x:xs) = foldl f arg xs
                            where arg = f initial x

foldl' :: (b -> a -> b) -> b -> [a] -> b
foldl' f initial [] = initial
foldl' f initial (x:xs) = arg `seq` foldl' f arg xs
                            where arg = f initial x

Форсирование вычислений невелирует накопление вычислений
Левые свертки наиболее эффективны
! Проблемы на бесконечных списках, для левой свертки !

НО ! Правая свертка может работать с бесконечными структурами:
-}
all :: (a -> Bool) -> [a] -> Bool
all p = foldr (\x b -> p x && b) True
{-
(&&) :: Bool -> Bool -> Bool
True && x = x
False && _ = False


all (<2) [1..] →
foldr (\x b -> x<2 && b) True [1..] →
foldr (\x b -> x<2 && b) True (1 : [2..]) →
(\x b -> x<2 && b) 1 (foldr (\x b -> x<2 && b) True [2..]) →
(1<2) && (foldr (\x b -> x<2 && b) True [2..]) →
True && (foldr (\x b -> x<2 && b) True [2..]) →
True && (foldr (\x b -> x<2 && b) True (2 : [3..])) →
True && ((\x b -> x<2 && b) 2 (foldr (\x b -> x<2 && b) True [4..])) →
True && ((2<2) && (foldr (\x b -> x<2 && b) True [3..])) →
True && (False && (foldr (\x b -> x<2 && b) True [3..])) →
False

-- foldr (-) 0 [1,2,3]
-- foldl (-) 0 [1,2,3]

-- partition (> 0) [1,2,-3,4,-5,-6]
-- find (> 0) [1,2,-3,4,-5,-6]
-- find (> 7) [1,2,-3,4,-5,-6]

-- takeWhile (/= "stop") ["hello", "send", "stop", "receive"]
-- span (/= "stop") ["hello", "send", "stop", "receive"]

-}

-- Версии сверток без начального значения для не пустых списков
{-
инициализирующее значение не нужно, тогда тип значения и тип результата должны совпадать 

foldr1 :: (a -> a -> a) -> [a] -> a
foldr1 _ [x] = x
foldr1 f (x:xs) = f x (foldr1 f xs)
foldr1 _ [] = error "foldr1: EmptyList"

foldl1 :: (a -> a -> a) -> [a] -> a
foldl1 f (x:xs) = foldl f x xs
foldl1 _ [] = error "foldl1: EmptyList"

-}

-- Как реализовать стандартную функцию reverse с использованием foldl? 



{-
-- Развертка -- операция двойственная к свертке
unfoldr :: (b -> Maybe (a, b)) -> b -> [a]
unfoldr g ini
            | Nothing <- next = []
            | Just (a,b) <- next = a : unfoldr g b
            where next = g ini

-- h b n = if n > b then Nothing else Just (n,succ n)
-- enumFT a b = unfoldr (h b) a
-- enumFT 3 10

-}

-----------------------------------------------------------------------------------------------------

-- {-# LANGUAGE ScopedTypeVariables #-}

-- import Data.Semigroup
-- import Data.List.NonEmpty

{-
Полугруппа — это множество с ассоциативной бинарной операцией над ним.

class Semigroup a where
    (<>) :: a -> a -> a
    {-# MINIMAL (<>) #-}

Закон для полугруппы:
(x <> y) <> z ≡ x <> (y <> z)

Список — полугруппа относительно конкатенации (++).
instance Semigroup [a] where
    (<>) = (++)

-- [1,2,3] <> [4,5,6]
-- [1,2,3] ++ [4,5,6]


class Semigroup a where
    (<>) :: a -> a -> a

    stimes :: Integral b => b -> a -> a
    stimes = stimesIdempotent
    -- stimes 5 "a" -- "a" <> "a" <> "a" <> "a" <> "a"
    
    sconcat :: Semigroup a => NonEmpty a -> a
    sconcat (a :| as) = go a as where
        go b (c:cs) = b <> go c cs
        go b [] = b
    -- sconcat $ "AB" :| ["CDE","FG"]

data NonEmpty a = a :| [a] — непустой список элементов типа a (гарантированно содержит хотя бы один элемент)
-- Оператор :| создает непустой список (NonEmpty) (объединяет первый элемент с остальными элементами списка)

-}


-----------------------------------------------------------------------------------------------------

-- import Data.Monoid
-- import Data.Coerce

{-
Моноид — это множество с ассоциативной бинарной операцией над ним и нейтральным элементом для этой операции.

class Semigroup a => Monoid a where
    mempty :: a
    
    mappend :: a -> a -> a
    mappend = (<>)
    
    mconcat :: [a] -> a
    mconcat = foldr mappend mempty

Законы для моноида:
mempty <> x ≡ x
x <> mempty ≡ x
(x `mappend` y) `mappend` z ≡ x `mappend` (y `mappend` z)

Список — моноид относительно конкатенации (++), нейтральный элемент — это пустой список.
instance Semigroup [a] where
    (<>) = (++)
instance Monoid [a] where
    mempty = []
    mconcat = concat
-- mconcat ["asd", "fgh", "jhkl"]


Bool - моноид относительно конъюнкции (&&) и дизъюнкции (||).

newtype All = All { getAll :: Bool } deriving (Eq, Ord, Read, Show, Bounded)
instance Semigroup All where
    All x <> All y = All (x && y)
instance Monoid All where
    mempty = True

newtype Any = Any { getAny :: Bool } deriving (Eq, Ord, Read, Show, Bounded)
instance Semigroup Any where
    Any x <> Any y = Any (x || y)
instance Monoid All where
    mempty = False

-- getAny . mconcat . map Any $ [False,False,True]
-- getAll . mconcat . map All $ [False,False,True]


Числа (Num) - моноид относительно:
    сложения (нейтральный элемент - 0)
    умножения (нейтральный элемент - 1)
    min (нейтральный элемент - maxBound)
    max (нейтральный элемент - minBound)

newtype Sum a = Sum { getSum :: a } deriving (Eq, Ord, Read, Show, Bounded, Num)
instance Num a => Semigroup (Sum a) where
    Sum x <> Sum y = Sum (x + y)
instance Num a => Monoid (Sum a) where
    mempty = Sum 0

-- Sum 3 <> Sum 2
-- Sum 2 * Sum 3 - Sum 5

newtype Product a = Product { getProduct :: a } deriving (Eq, Ord, Read, Show, Bounded, Num)
instance Num a => Semigroup (Product a) where
    (<>) = coerce ((*) :: a -> a -> a) -- Data.Coerce
instance Num a => Monoid (Product a) where
    mempty = Product 1

-- Product 3 <> Product 2


newtype Min a = Min { getMin :: a } deriving (Eq, Ord, Read, Show, Bounded)
instance Ord a => Semigroup (Min a) where
    (<>) = coerce (min :: a -> a -> a)
    stimes = stimesIdempotent
instance (Ord a, Bounded a) => Monoid (Min a) where
    mempty = maxBound

-- Min "Hello" <> Min "Hi"
-- (getMin . mconcat . map Min) [7,3,2,12] :: Int
-- (getMin . mconcat . map Min) [] :: Int

-}

data Color = Red | Yellow | Blue | Green | Purple | Orange | Brown deriving (Show, Eq)


instance Semigroup Color where
  (<>) Red Blue = Purple
  (<>) Blue Red = Purple
  (<>) Yellow Blue = Green
  (<>) Blue Yellow = Green
  (<>) Yellow Red = Orange
  (<>) Red Yellow = Orange
  (<>) a b = if a == b then a else Brown
-- Red <> Yellow
-- Red <> Blue
-- Green <> Purple
-- (Green <> Blue) <> Yellow
-- Green <> (Blue <> Yellow)

{-
instance Semigroup Color where
  (<>) Red Blue = Purple
  (<>) Blue Red = Purple
  (<>) Yellow Blue = Green
  (<>) Blue Yellow = Green
  (<>) Yellow Red = Orange
  (<>) Red Yellow = Orange
  (<>) a b | a == b = a
           | all (`elem` [Red, Blue, Purple]) [a, b]   = Purple
           | all (`elem` [Blue, Yellow, Green]) [a, b] = Green
           | all (`elem` [Red, Yellow, Orange]) [a, b] = Orange
           | otherwise = Brown
-}
-- (Green <> Blue) <> Yellow
-- Green <> (Blue <> Yellow)

-----------------------------------------------------------------------------------------------------

-- import Data.Functor
-- import Data.Foldable

{-
Функтор - тип, который поддерживает операцию применения функции к значению, находящемуся внутри контекста, без изменения структуры контекста.
Поднимает стрелку на уровень контейнера.

class Functor f where
    fmap :: (a -> b) -> f a -> f b
-- :t map

Законы функтора:
fmap id = id (сохранение тождественного морфизма)
fmap (f . g) = fmap f . fmap g (сохранение композиции)

Список - функтор
instance Functor [] where
    fmap f [] = []
    fmap f (x:xs) = f x : fmap f xs
-- fmap (+1) [1, 2, 3]

Maybe - функтор
instance Functor Maybe where
    fmap f Nothing = Nothing
    fmap f (Just x) = Just (f x)
-- fmap (+1) (Just 5)
-- fmap (+1) Nothing

Дерево - функтор
-}

data Tree a = Leaf a | Branch (Tree a) (Tree a) deriving Show

instance Functor Tree where
    fmap f (Leaf x) = Leaf (f x)
    fmap f (Branch l r) = Branch (fmap f l) (fmap f r)
-- fmap (+1) (Branch (Leaf 1) (Leaf 2))
-- (+1) <$> (Branch (Leaf 1) (Leaf 2))

{-

class Functor f where
    fmap :: (a -> b) -> f a -> f b
    (<$) :: a -> f b -> f a
    (<$) = fmap . const

(<$>) :: Functor f => (a -> b) -> f a -> f b
(<$>) = fmap
($>) :: Functor f => f a -> b -> f b
($>) = flip (<$)

-- Just 42 $> "foo"
-- Nothing $> "foo"
-- Nothing <$ "foo"

(<&>) :: Functor f => f a -> (a -> b) -> f b
xs <&> f = f <$> xs

-- (+10) <$> (^2) <$> [1,2,3]
-- [1,2,3] <&> (^2) <&> (+10)


Представители Functor для двухпараметрических типов (:: * -> * -> *)

instance Functor (Either e) where
    fmap :: (a -> b) -> Either e a -> Either e b
    fmap _ (Left x) = Left x
    fmap g (Right y) = Right (g y)

instance Functor ((,) s) where
    fmap :: (a -> b) -> (s,a) -> (s,b)
    fmap g (x,y) = (x, g y)

instance Functor ((->) e) where
    fmap :: (a -> b) -> (->) e a -> (->) e b
         -- (a -> b) -> (e -> a) -> (e -> b)
    fmap = (.) -- f . g = f (g x) -- композиция

f <$> g <$> xs ≡ (f <$> g) <$> xs ≡ (f . g) <$> xs ≡ fmap (f . g) xs


Не все типы можно сделать представителями функтора (сложности возникают со стрелочными типами):

-- Эндоморфизм:
newtype Endo a = Endo { appEndo :: a -> a }
instance Functor Endo where
    fmap :: (a -> b) -> Endo a -> Endo b
    fmap _ (Endo _) = Endo id
-- (a -> b) -> (a -> a) -> b -> b

-- Флипнутая стрелка:
newtype RevArr c a = RevArr { appRevArr :: a -> c }
-- (a -> b) -> (a -> с) -> b -> с
-}

-----------------------------------------------------------------------------------------------------

{-
Класс Foldable

class Foldable t where
    fold :: Monoid m => t m -> m
    fold = foldMap id

    foldMap :: Monoid m => (a -> m) -> t a -> m
    foldMap f = foldr (mappend . f) mempty
        -- f :: a -> m
        -- mappend :: m -> (m -> m)
        -- (mappend . f) :: a -> (m -> m)
        -- foldr :: (a -> b -> b) -> b -> t a -> b

    foldr, foldr' :: (a -> b -> b) -> b -> t a -> b
    foldr f z t = appEndo (foldMap (Endo . f) t) z

    foldl, foldl' :: (a -> b -> a) -> a -> t b -> a
    foldl f z t = appEndo (getDual (foldMap (Dual . Endo . flip f) t)) z

    ...

-- fold [Sum 1, Sum 2, Sum 3, Sum 4]
-- getSum $ foldMap (Sum . (^2)) [1, 2, 3, 4]
-- foldr (+) 0 [1, 2, 3, 4]
-- foldl (*) 1 [1, 2, 3, 4]
-- foldr1 max [1, 2, 3, 4]
-- foldl1 min [1, 2, 3, 4]

instance Foldable [] where ... стандартная реализация

instance Foldable Maybe where
    foldr _ z Nothing = z
    foldr f z (Just x) = f x z
    foldl _ z Nothing = z
    foldl f z (Just x) = f z x

-- Функции из полного определения класса Foldable
class Foldable t where
    toList :: t a -> [a] -- любой контейнер Foldable может быть сведен к списку
    
    null :: t a -> Bool
    null = foldr (\_ _ -> False) True
    
    length :: t a -> Int
    length = foldl' (\n _ -> n + 1) 0
    
    elem :: Eq a => a -> t a -> Bool

Законы Foldable:
foldr f z t ≡ appEndo (foldMap (Endo . f) t ) z
foldl f z t ≡ appEndo (getDual (foldMap (Dual . Endo . flip f) t)) z
fold ≡ foldMap id 
length ≡ getSum . foldMap (Sum . const 1)
sum ≡ getSum . foldMap Sum
product ≡ getProduct . foldMap Product
minimum ≡ getMin . foldMap Min
maximum ≡ getMax . foldMap Max
foldr f z ≡ foldr f z . toList
foldl f z ≡ foldl f z . toList


Двухпараметрические представители Foldable:

instance Foldable (Either a) where
    foldMap _ (Left _) = mempty
    foldMap f (Right y) = f y
    foldr _ z (Left _) = z
    foldr f z (Right y) = f y z
    length (Left _) = 0
    length (Right _) = 1
    null = isLeft

foldMap :: (b -> m) -> Either a b -> m

-- maximum (Right 37)
-- maximum (Left 37)

instance Foldable ((,) a) where
    foldMap f (_,y) = f y
    foldr f z (_,y) = f y z
    length _ = 1
    null _ = False

foldMap :: (b -> m) -> (,) a b -> m

-- foldr (+) 5 ("Answer",37)
-- maximum (100,42)

-}

-----------------------------------------------------------------------------------------------------

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

-----------------------------------------------------------------------------------------------------

{-
($)   ::   (a -> b) ->    a ->    b
map   ::   (a -> b) -> [] a -> [] b
(<$>) ::   (a -> b) -> f  a -> f  b
(<*>) :: f (a -> b) -> f  a -> f  b
-}

-----------------------------------------------------------------------------------------------------


