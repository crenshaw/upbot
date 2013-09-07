{--
 - readFile.hs
 -
 - Read from a file (config) and print its contents to STDOUT.
 -
 - @author Fernando Freire
 - @since  08 July 2013
 -
 --}

import System.Environment

main = do
    s <- readFile "./config"
    putStrLn s
