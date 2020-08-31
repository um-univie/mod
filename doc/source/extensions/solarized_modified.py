# -*- coding: utf-8 -*-
"""
    A modified version of pygments.styles.solarized
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"""

from pygments.style import Style
from pygments.token import Comment, Error, Generic, Keyword, Name, Number, \
    Operator, String, Token


def make_style(colors):
    return {
        Token:               colors['base01'],

        Comment:             'italic ' + colors['blue'],
        Comment.Hashbang:    colors['base01'],
        Comment.Multiline:   colors['base01'],
        Comment.Preproc:     'noitalic ' + colors['magenta'],
        Comment.PreprocFile: 'noitalic ' + colors['base01'],

        Keyword:             colors['yellow'],
        Keyword.Constant:    colors['cyan'],
        Keyword.Declaration: colors['cyan'],
        Keyword.Namespace:   colors['orange'],
        Keyword.Type:        colors['yellow'],

        Operator:            colors['base01'],
        Operator.Word:       colors['yellow'],

        Name.Builtin:        colors['cyan'],
        Name.Builtin.Pseudo: colors['cyan'],
        Name.Class:          colors['cyan'],
        Name.Constant:       colors['cyan'],
        Name.Decorator:      colors['cyan'],
        Name.Entity:         colors['cyan'],
        Name.Exception:      colors['green'],
        Name.Function:       colors['cyan'],
        Name.Function.Magic: colors['cyan'],
        Name.Label:          colors['cyan'],
        Name.Namespace:      colors['cyan'],
        Name.Tag:            colors['cyan'],
        Name.Variable:       colors['cyan'],
        Name.Variable.Global:colors['cyan'],
        Name.Variable.Magic: colors['cyan'],

        String:              colors['red'],
        String.Doc:          colors['base01'],
        String.Regex:        colors['orange'],

        Number:              colors['red'],

        Generic.Deleted:     colors['red'],
        Generic.Emph:        'italic',
        Generic.Error:       colors['red'],
        Generic.Heading:     'bold',
        Generic.Subheading:  'underline',
        Generic.Inserted:    colors['green'],
        Generic.Strong:      'bold',
        Generic.Traceback:   colors['blue'],

        Error:               'bg:' + colors['red'],
    }


LIGHT_COLORS = {
    'base03':  '#002b36',
    'base02':  '#073642',
    'base01':  '#586e75',
    'base00':  '#657b83',
    'base0':   '#839496',
    'base1':   '#93a1a1',
    'base2':   '#eee8d5',
    'base3':   '#fdf6e3',
    'yellow':  '#b58900',
    'orange':  '#cb4b16',
    'red':     '#dc322f',
    'magenta': '#d33682',
    'violet':  '#6c71c4',
    'blue':    '#268bd2',
    'cyan':    '#2aa198',
    'green':   '#859900',
}


class light(Style):
    """
    The solarized style, light. Modified.
    """

    styles = make_style(LIGHT_COLORS)
    background_color = LIGHT_COLORS['base3']
    highlight_color = LIGHT_COLORS['base2']
