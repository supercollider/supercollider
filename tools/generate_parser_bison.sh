#!/bin/sh
# NOTE: this command is a copied in the pre-commit hook
bison -Werror --defines=lang/LangSource/Bison/lang11d_tab.h --output=lang/LangSource/Bison/lang11d_tab.cpp lang/LangSource/Bison/lang11d
