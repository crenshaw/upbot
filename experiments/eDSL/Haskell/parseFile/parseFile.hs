{--
 - parseFile.hs
 -
 - Parse a config file and prints its contents to STDOUT in the following format
 -
 - EVENT: eventName
 - RESPO: responseName
 - -------------------
 -
 - @author Fernando Freire
 - @since 08 July 2013
 --}

import Data.List.Split

main = do
    file <- readFile "config"
    mapM_ (putStrLn) (splitOn ":" file)
