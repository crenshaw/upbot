" Vim syntax file
" Language: Maritime
" Maintainer: Fernando Freire
" Latest Revision: 09 July 2013

if exists("b:current_syntax")
    finish
endif

" Keywords
syn region stateString start='"' end='"' contained
syn match stateLabel '*[a-zA-Z]' nextgroup=stateString
syn keyword basicLanguageKeywords define state
