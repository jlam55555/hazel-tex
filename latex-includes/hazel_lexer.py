from pygments.lexer import RegexLexer, words
from pygments.token import *

class HazelLexer(RegexLexer):
    """
    Very simple lexer for the Hazel experimental structured programming language.

    Structure based off of the Go language lexer.
    https://github.com/pygments/pygments/blob/master/pygments/lexers/go.py
    """
    
    name = 'Hazel'
    aliases = ['hazel']
    filenames = ['*.hz']

    tokens = {
        'root': [
            # whitespace
            (r'\n', Whitespace),
            (r'\s+', Whitespace),
            # keywords
            (words(('let', 'in', 'of', 'case'), suffix=r'\b'), Keyword),
            (words(('Int', 'Bool'), suffix=r'\b'), Keyword.Type),
            (r'inj\[(R|L)\]', Keyword),
            # literals
            (r'(true|false|\[\])\b', Keyword.Constant),
            (r'\d+', Number),
            (r'\d+\.\d*', Number),
            (r'\d*\.\d+', Number),
            # Tokens
            (r'[+\-*/<>=]|<|>|==|\+.|\-.|\*.|\/.|<.|>.|==.|::', Operator),
            (r'[.,<>(){}|:]', Punctuation),
            # identifier
            (r'[^\W\d]\w*', Name.Other),
        ]
    }
