// Copyright Jordan Henderson 2026
%require "3.8"
%language "C++"

%define api.value.type variant
%define api.token.prefix {TOKEN_}
%define api.namespace {sc::ast::parser}
%define parse.error custom
// Lookahead correction, so the expected tokens in an error are exact, despite default reductions. It also lets the lexer
// ask whether the parser can accept a token, see yylex.
%define parse.lac full

%locations
%define api.location.type { sc::lex::SourceCodeRange }


// This goes in the header
%code requires
{

#include "parser_context.hpp"
#include "sc_parser/sc_grammar_shared.hpp"

}

// These are member variables in the parser class
%parse-param {ParserContext& cxt}

// These are arguments to the function yylex
%lex-param {ParserContext& cxt}

// This goes at the top of the source file
%code top
{

#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/nodes.hpp"

#include "sc_grammar_parser.hpp"
#include "sc_grammar_impl.hpp"
#include "parser_context.hpp"


using namespace sc::ast;


}




// There is still location data, just no semantic value (the token tag is all the data you need)
%token <LexerToken> REGION_SEPARATOR
%token <LexerToken> OPENCURLY CLOSECURLY OPENSQUARE CLOSESQUARE OPENPAREN CLOSEPAREN 
%token <LexerToken> SEMICOLON NONLOCALRETURN COMMA HASH TILDE

// TODO: these literal tokens should be expand into all the types the lexer recognizes and turned into nodes (lower case versions).
%token <LexerToken> NAME INTEGER INTEGER_RADIX HEXADECIMAL FLOAT FLOAT_RADIX FLOAT_EXPONENT FLOAT_INF ACCIDENTAL_STEPS ACCIDENTAL_CENTS SYMBOL_QUOTE SYMBOL_SLASH STRINGLINE ASCII PRIMITIVENAME CLASSNAME CURRYARG 
%token <LexerToken> VAR ARG CLASSVAR CONST
%token <LexerToken> NIL TRUE FALSE PI
%token <LexerToken> ELLIPSIS DOTDOT BEGINCLOSEDFUNC
%token <LexerToken> BADTOKEN INTERPRET
%token <LexerToken> LEFTARROW 
%token <LexerToken> WHILE
%token <LexerToken> LEXER_ERROR

%left  <LexerToken> COLON
%right <LexerToken> EQUALSSIGN
%left  <LexerToken> BINOP KEYBINOP MINUS LESSTHAN GREATERTHAN MULTIPLY ADD PIPE READWRITEVAR
%left  <LexerToken> DOT
%right <LexerToken> BACKTICK
%right <LexerToken> UMINUS

////////////////////////////////////////////////////////////////////////////////
// types
////////////////////////////////////////////////////////////////////////////////

%type <GoIndex> go

%type <RegionListIndex> region
%type <RegionItemIndex> region.item 

%type <ClassOrExtensionListIndex> classOrExtList.list
%type <ClassOrExtensionIndex> classOrExtList.item

%type <ClassExtensionIndex> class.extension 
%type <ClassIndex> class

%type <maybe<ClassNameIdentifierIndex>> class.super.opt
%type <maybe<NamedIdentifierIndex>> class.slot.opt

%type <BigMemberListIndex> members members.opt
%type <DeclareAnyMemberListIndex> members.entry

%type <DeclareMemberListIndex> members.entry.list

%type <DeclareMemberIndex> members.entry.item 

%type <SelectorIndex> method.name
%type <AnyMethodIndex> method  method.base
%type <MethodListIndex> method.list method.list.opt

%type <AnyExprIndex> msgsend // not all msgsends result in a message node!

%type<ArgumentEntryIndex> arguments.entries
%type<ArgumentListIndex> arguments arguments.no_trailing arguments.paren arguments.maybe_paren

%type <DeclareArgumentListIndex> argument_declarations.list argument_declarations argument_declarations.pipelist argument_declarations.opt

%type <DeclareVariableListIndex> variable_declarations.list variable_declarations
%type <DeclareAnyVariableIndex> variable_declarations.list.item

%type<BlockListIndex> block.opt_list block.list

%type <AnyExprIndex> expr expr.base
%type <AnyExprIndex> expr_or_seq expr_or_seq.base 

%type <NamedIdentifierIndex> name 

%type <SelectorIndex> binary_op.no_adverb binary_op.raw
%type <SelectorMaybeAdverbIndex> binary_op
%type <AdverbIndex> adverb

%type <GeneratorAnyIndex> generator
%type <GeneratorQualAnyIndex> generator.qual
%type <GeneratorQualListIndex> generator.qual.list

// literals
%type <AnyPossiblyLiteralIndex> literal literal.terminal 

%type <ArrayIndex> literal.array literal.array.contents
%type <DictionaryIndex> literal.dictionary literal.dictionary.entries
%type <DictionaryEntryIndex> literal.dictionary.entry

%type <NameListIndex> name.list
%type <NamePackIndex> name.pack

%type <BlockItemIndex> block.contents.item
%type <BlockContentsListIndex> block.contents 
%type <BlockIndex> block 
%type <NilLitIndex> nil
%type <BooleanLitIndex> boolean
%type <AnySymbolIndex> symbol
%type <StringLitIndex> string
%type <IntLitIndex> integer
%type <FloatLitIndex> float.raw float.raw_unsigned
%type <AccidentalIndex> accidental accidental.unsigned
%type <FloatProducingIndex> float
%type <ASCIIIndex> ascii

%type <RangeIndex> range 
%type <AtMessageIndex> expr.base.range expr.dotrange
%type <RangeGeneratorIndex> range.generator
%type <RangeAssignIndex> expr.dotrange.assign expr.base.range.assign


// misc
%type <RWAccessorIndex> accessor 

%type <FileAsFunctionIndex> file_as_func


////////////////////////////////////////////////////////////////////////////////
// start rule
////////////////////////////////////////////////////////////////////////////////

%start go


////////////////////////////////////////////////////////////////////////////////
// rules
////////////////////////////////////////////////////////////////////////////////

%%

go 
	: region semicolon.opt YYEOF { $$ = cxt.graph->assign_root($region); }
	| INTERPRET YYEOF { $$ = cxt.graph->assign_root(cxt.create(RegionList{}, @$)); }
	| file_as_func { $$ = cxt.graph->assign_root($1); }
	| classOrExtList.list[classes] YYEOF  { $$ = cxt.graph->assign_root($classes); }
	| %empty { $$ = cxt.graph->assign_root(cxt.create(ClassOrExtensionList{}, @$));}
	;

////////////////////////////////////////////////////////////////////////////////
// file as function
////////////////////////////////////////////////////////////////////////////////

file_as_func 
	: INTERPRET argument_declarations[args] block.contents[contents] semicolon.opt YYEOF
		{ $$ = cxt.create(FileAsFunction{}, @$, $args, $contents); }

	| INTERPRET variable_declarations[decls] SEMICOLON block.contents[contents] semicolon.opt YYEOF
		{ $$ = cxt.create(FileAsFunction{}, @$, cxt.create(DeclareArgumentList{}, @1), cxt.graph->prepend($contents, $decls)); }

	| INTERPRET variable_declarations[decls] semicolon.opt YYEOF
		{ $$ = cxt.create(FileAsFunction{}, @$, cxt.create(DeclareArgumentList{}, @1), cxt.create(BlockContentsList{}, @3, $decls)); }
	;

////////////////////////////////////////////////////////////////////////////////
// region
////////////////////////////////////////////////////////////////////////////////

region.item
	: expr
		{ $$ = $expr; } 
	// Parsed so it can be reported as a return in interpreted code, rather than an unexpected token.
	| NONLOCALRETURN expr
		{ $$ = cxt.create(NonLocalReturnExpr{}, range(@NONLOCALRETURN, @expr), $expr); }
	| OPENPAREN argument_declarations[args] block.contents[contents] semicolon.opt CLOSEPAREN 
		{ $$ = cxt.create(ImmediatelyEvaluatedBlock{}, @$, $args, $contents); }

	| OPENPAREN argument_declarations[args] CLOSEPAREN 
		{ $$ = cxt.create(ImmediatelyEvaluatedBlock{}, @$, $args, cxt.create(BlockContentsList{}, @3)); }

	| OPENPAREN variable_declarations[decls] SEMICOLON block.contents[contents] semicolon.opt CLOSEPAREN  
		{ $$ = cxt.create(ImmediatelyEvaluatedBlock{}, @$, cxt.create(DeclareArgumentList{}, @1), cxt.graph->prepend($contents, $decls)); }
	;

region
	: INTERPRET region.item[item]
		{ $$ = cxt.create(RegionList{}, @$, $item); }

	| INTERPRET error
		{
			error_recovery::expr(cxt);
			$$ = cxt.create(RegionList{}, @$, cxt.create_error(@$, @error));
		}

	| region[r] SEMICOLON region.item[item]
		{ $$ = cxt.graph->append($r, @$, $item); }

	| region[r] REGION_SEPARATOR region.item[item]
		{ $$ = cxt.graph->append($r, @$, $item); }

	| region[r] error[e] 
		{
			// A missing semicolon between lines is inserted by the lexer, see yylex.
			error_recovery::expr(cxt);
			$$ = cxt.graph->append($r, cxt.create_error(@e, @e, Index { *$r }));
			yyclearin;
		}
	;




////////////////////////////////////////////////////////////////////////////////
// class or class extension list
////////////////////////////////////////////////////////////////////////////////

classOrExtList.list
	: classOrExtList.item[item]
		{ $$ = cxt.create(ClassOrExtensionList{}, @$, $item); }
	| classOrExtList.list[list] classOrExtList.item[item]
		{ $$ = cxt.graph->append($list, $item); }
	;

classOrExtList.item
	: class { $$ = $1; }
	| class.extension { $$ = $1; }
	;

class 
	: CLASSNAME[name] class.slot.opt[slot] class.super.opt[super] OPENCURLY members.opt[vars] method.list.opt[meths] CLOSECURLY
		{ $$ = cxt.create(Class{ cxt.take_version(@OPENCURLY) }, @$, cxt.create(ClassNameIdentifier{}, @name), $slot, $super, $vars, $meths); }
	;

class.super.opt 
	: %empty { $$ = cxt.create(Missing{}, @$); } 
	| COLON CLASSNAME { $$ = cxt.create(ClassNameIdentifier{}, @CLASSNAME); };
	;

class.slot.opt 
	: %empty { $$ = cxt.create(Missing{}, @$); } 
	| OPENSQUARE name CLOSESQUARE { $$ = $name; };
	| OPENSQUARE CLOSESQUARE { $$ = cxt.create(Missing{}, @$); };
	;

class.extension 
	: ADD CLASSNAME[name] OPENCURLY method.list.opt[meths] CLOSECURLY
		{ $$ = cxt.create(ClassExtension{ cxt.take_version(@OPENCURLY) }, @$, cxt.create(ClassNameIdentifier{}, @name), $meths); }
	;

////////////////////////////////////////////////////////////////////////////////
// members
////////////////////////////////////////////////////////////////////////////////

members.entry.item 
	: accessor variable_declarations.list.item[decl] 
		{ $$ = cxt.create(DeclareMember{}, @$, $accessor, $decl); }
	;

members.entry.list
	: members.entry.item 
		{ $$ = cxt.create(DeclareMemberList{}, @$, $1); }
	| members.entry.list COMMA members.entry.item
		{ $$ = cxt.graph->append($1, $3); }
	;

members.entry
	: CLASSVAR members.entry.list[vars]
		{ $$ = cxt.graph->cast<DeclareClassMemberList>($vars); }
	| VAR members.entry.list[vars]
		{ $$ = cxt.graph->cast<DeclareMemberList>($vars); }
	| CONST members.entry.list[vars]
		{ $$ = cxt.graph->cast<DeclareConstList>($vars); }
	;

members
	: members.entry 
		{ $$ = cxt.create(BigMemberList{}, @$, $1);}
	| members SEMICOLON members.entry
		{ $$ = cxt.graph->append($1, $3); }
	;

members.opt
	: %empty { $$ = cxt.create(BigMemberList{}, @$); }
	| members semicolon.opt { $$ = $1; }
	;

////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////

method.name 
	: name { $$ = $1; }
	| binary_op.raw { $$ = $1; }
	;

method.base
	: method.name[name] OPENCURLY argument_declarations.opt[args] block.contents[block] semicolon.opt CLOSECURLY
		{ $$ = cxt.create(Method{}, @$, $name, $args, cxt.create(Missing{},@2), $block); }

	| method.name[name] OPENCURLY argument_declarations.opt[args] CLOSECURLY
		{ $$ = cxt.create(Method{}, @$, $name, $args, cxt.create(Missing{},@2), cxt.create(BlockContentsList{}, @4)); }

	| method.name[name] OPENCURLY argument_declarations.opt[args] PRIMITIVENAME[prim] semicolon.opt block.contents[block] semicolon.opt CLOSECURLY
		{ $$ = cxt.create(Method{}, @$, $name, $args, cxt.create(PrimitiveIdentifier{}, @prim), $block); }

	| method.name[name] OPENCURLY argument_declarations.opt[args] PRIMITIVENAME[prim] semicolon.opt CLOSECURLY
		{ $$ = cxt.create(Method{}, @$, $name, $args, cxt.create(PrimitiveIdentifier{}, @prim), cxt.create(BlockContentsList{}, @5)); }
	
	| method.name[name] OPENCURLY[o] error[e] 
		{
			error_recovery::method(cxt, @o); // advance over the closing curly of function.
			// The name is left on the stack, so isn't discarded.
			$$ = cxt.graph->prepend(cxt.create_error(@$, @e), $name);
			yyclearin;
		}
	;

method
	: method.base { $$ = $1; }
	| MULTIPLY method.base[meth] 
		{ 
			if (auto c = cxt.graph->as_a<MethodIndex>($meth)) {
				$$ = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location($meth) = @$; 
			$$ = $meth; 
		}
	;

method.list 		
	: method { $$ = cxt.create(MethodList{}, @$, $1); }
	| method.list method { $$ = cxt.graph->append($1, @$, $2); }
	;

method.list.opt 
	: %empty { $$ = cxt.create(MethodList{}, @$); }
	| method.list { $$ = $1; }
	;


////////////////////////////////////////////////////////////////////////////////
// block and block lists
////////////////////////////////////////////////////////////////////////////////

block.open : OPENCURLY | BEGINCLOSEDFUNC;

block	
	: block.open argument_declarations.opt[args] block.contents[content] semicolon.opt CLOSECURLY 
		{ $$ = cxt.create(BlockNode{}, @$, $args, $content); }
	| block.open argument_declarations.opt[args] CLOSECURLY 
		{ $$ = cxt.create(BlockNode{}, @$, $args, cxt.create(BlockContentsList{}, @$)); }
	;

block.opt_list	
	: %empty { $$ = cxt.create(BlockList{}, @$); } 
	| block.list { $$ = $1; } 
	;

block.list 		
	: block { $$ = cxt.create(BlockList{}, @$, $1); }
	| generator { $$ = cxt.create(BlockList{}, @$, $1); }
	| block.list block { $$ = cxt.graph->append($1, @$, $2); }
	| block.list generator { $$ = cxt.graph->append($1, @$, $2); }
	;

block.contents
	: block.contents.item { $$ = cxt.create(BlockContentsList{}, @$, $1); }
	| block.contents SEMICOLON block.contents.item { $$ = cxt.graph->append($1, @$, $3); }
	| block.contents NONLOCALRETURN expr 
		{ 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(@NONLOCALRETURN, @expr), $expr);
			$$ = cxt.graph->append($1, @$, non_local); 
		}
	| block.contents SEMICOLON NONLOCALRETURN expr 
		{ 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(@NONLOCALRETURN, @expr), $expr);
			$$ = cxt.graph->append($1, @$, non_local); 
		}
	| NONLOCALRETURN expr 
		{ 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(@NONLOCALRETURN, @expr), $expr);
			$$ = cxt.create(BlockContentsList{}, @$, non_local);
		}
	;

block.contents.item
	: expr { $$ = $1; }
	| variable_declarations { $$ = $1; }
	;


////////////////////////////////////////////////////////////////////////////////
// msgsends
////////////////////////////////////////////////////////////////////////////////

adverb  
	: DOT name { $$ = $2; }
	| DOT integer { $$ = $2; }
	| DOT OPENPAREN expr_or_seq CLOSEPAREN { $$ = cxt.create(AdverbExprNode{}, @$, $3);  };
	;

binary_op.raw	
	: BINOP { $$ = cxt.create(SelectorNode{ false }, @$); }	
	| READWRITEVAR { $$ = cxt.create(SelectorNode{ false }, @$); }	
	| LESSTHAN { $$ = cxt.create(SelectorNode{ false }, @$); }	
	| GREATERTHAN { $$ = cxt.create(SelectorNode{ false }, @$); }	
	| MINUS { $$ = cxt.create(SelectorNode{ false }, @$); }	
	| MULTIPLY { $$ = cxt.create(SelectorNode{ false }, @$); }	
	| ADD { $$ = cxt.create(SelectorNode{ false }, @$); }	
	| PIPE { $$ = cxt.create(SelectorNode{ false }, @$); }	
	;

binary_op.no_adverb 	
	: binary_op.raw   { $$ = $1; }
	| KEYBINOP { $$ = cxt.create(SelectorNode{ true }, @$); } 
	;

binary_op	
	: binary_op.no_adverb adverb
		{ $$ = cxt.create(SelectorWAdverb{}, @$, $1, $2); }
	| binary_op.no_adverb
		{ $$ = $1; }
	;

msgsend 
	: OPENPAREN binary_op.no_adverb[selector] CLOSEPAREN OPENPAREN arguments[args] CLOSEPAREN block.opt_list[blocks]
		{ $$ = cxt.create(PrefixMessage{}, @$, $selector, $args, $blocks); }

	| OPENPAREN binary_op.no_adverb[selector] CLOSEPAREN OPENPAREN CLOSEPAREN block.list[blocks]
		{ $$ = cxt.create(PrefixMessage{}, @$, $selector, cxt.create(ArgumentList{}, @4), $blocks); }

	| OPENPAREN binary_op.no_adverb[selector] CLOSEPAREN block.list[blocks]
		{ $$ = cxt.create(PrefixMessage{}, @$, $selector, cxt.create(ArgumentList{}, @4), $blocks); }


	| name[selector] OPENPAREN arguments[args] CLOSEPAREN block.opt_list[blocks]
		{ $$ = cxt.create(PrefixMessage{}, @$, $selector, $args, $blocks); }

	| name[selector] OPENPAREN CLOSEPAREN block.list[blocks]
		{ $$ = cxt.create(PrefixMessage{}, @$, $selector, cxt.create(ArgumentList{}, @2), $blocks); }

	| name[selector] block.list[blocks]
		{ $$ = cxt.create(PrefixMessage{}, @$, $selector, cxt.create(ArgumentList{}, @2), $blocks); }
	
	| expr DOT name[selector] arguments.maybe_paren[args] block.opt_list[blocks]
		{ $$ = cxt.create(PostfixMessage{}, @$, $expr, $selector, $args, $blocks); }

	| expr DOT arguments.paren[args] block.opt_list[blocks]
		{ $$ = cxt.create(ValueMessage{}, @$, $expr, $args, $blocks); }

	| expr DOT OPENPAREN CLOSEPAREN block.opt_list[blocks]
		{ $$ = cxt.create(ValueMessage{}, @$, $expr, cxt.create(ArgumentList{}, @3), $blocks); }
		
//	| expr DOT error 
//		{
//			auto unexpected = cxt.consume_error(); 
//			std::cout << "GOT AN ERROR WITH A DOT"<< std::endl;
//			$$ = $1;
//		}

	| CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
		{  $$ = cxt.create(CollectionNode{}, @$, cxt.create(ClassNameIdentifier{}, @1), $3); }
	
	| CLASSNAME block.list[blocks]
		{ 
			auto class_name = cxt.create(ClassNameIdentifier{}, @1);
			$$ = cxt.create(NewMessage{}, @$, class_name, cxt.create(ArgumentList{}, @1), $blocks);
		}
	| CLASSNAME arguments.paren[args] block.opt_list[blocks]
		{ 
			auto class_name = cxt.create(ClassNameIdentifier{}, @1);
			$$ = cxt.create(NewMessage{}, @$, class_name, $args, $blocks);
		}

	| CLASSNAME OPENPAREN CLOSEPAREN block.opt_list[blocks]
		{ 
			auto class_name = cxt.create(ClassNameIdentifier{}, @1);
			$$ = cxt.create(NewMessage{}, @$, class_name, cxt.create(ArgumentList{}, @1), $blocks);
		}
	;

////////////////////////////////////////////////////////////////////////////////
// Ranges
////////////////////////////////////////////////////////////////////////////////
// This is a bit verbose to avoid conflicts in the grammar.
// (1, 2...3) and (:1, 2 ... 3) explicitly state how many arguments go before the '...'.
// The others use an argument pack and can have as many as needed.

expr.dotrange
	: expr[receiver] DOT OPENSQUARE[o] arguments[args] DOTDOT expr_or_seq[e] CLOSESQUARE[c]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, $args, $e);
			$$ = cxt.create(AtMessage{}, @$, $receiver, r);
		}

	| expr[receiver] DOT OPENSQUARE[o] DOTDOT expr_or_seq[e] CLOSESQUARE[c]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, @o),  $e);
			$$ = cxt.create(AtMessage{}, @$, $receiver, r);
		}

	| expr[receiver] DOT OPENSQUARE[o] arguments[args] DOTDOT CLOSESQUARE[c]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, $args, cxt.create(Missing{}, @5));
			$$ = cxt.create(AtMessage{}, @$, $receiver, r);
		}
	;

expr.dotrange.assign
	: expr[receiver] DOT OPENSQUARE[o] arguments[args] DOTDOT expr_or_seq[e] CLOSESQUARE[c] EQUALSSIGN expr[assign]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, $args, $e);
			$$ = cxt.create(RangeAssign{}, @$, $receiver, r, $assign);
		}

	| expr[receiver] DOT OPENSQUARE[o] arguments[args] DOTDOT CLOSESQUARE[c] EQUALSSIGN expr[assign]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, $args, cxt.create(Missing{}, @5));
			$$ = cxt.create(RangeAssign{}, @$, $receiver, r, $assign);
		}

	| expr[receiver] DOT OPENSQUARE[o] DOTDOT expr_or_seq[e] CLOSESQUARE[c] EQUALSSIGN expr[assign]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, @o),  $e);
			$$ = cxt.create(RangeAssign{}, @$, $receiver, r, $assign);
		}
	;


expr.base.range
	: expr.base[receiver] OPENSQUARE[o] arguments[args] DOTDOT expr_or_seq[e] CLOSESQUARE[c]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, $args, $e);
			$$ = cxt.create(AtMessage{}, @$, $receiver, r);
		}
	| expr.base[receiver] OPENSQUARE[o] arguments[args] DOTDOT CLOSESQUARE[c]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, $args, cxt.create(Missing{}, @5));
			$$ = cxt.create(AtMessage{}, @$, $receiver, r);	
		}
	| expr.base[receiver] OPENSQUARE[o] DOTDOT expr_or_seq[e] CLOSESQUARE[c]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, @o),  $e);
			$$ = cxt.create(AtMessage{}, @$, $receiver, r);	
		}
	;

expr.base.range.assign
	: expr.base[receiver] OPENSQUARE[o] arguments[args] DOTDOT expr_or_seq[e] CLOSESQUARE[c] EQUALSSIGN expr[assign]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, $args, $e);
			$$ = cxt.create(RangeAssign{}, @$, $receiver, r, $assign);
		}
	| expr.base[receiver] OPENSQUARE[o] arguments[args] DOTDOT CLOSESQUARE[c] EQUALSSIGN expr[assign]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, $args, cxt.create(Missing{}, @4));
			$$ = cxt.create(RangeAssign{}, @$, $receiver, r, $assign);
		}
	| expr.base[receiver] OPENSQUARE[o] DOTDOT expr_or_seq[e] CLOSESQUARE[c] EQUALSSIGN expr[assign]
		{
			const auto loc = range(@o, @c);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, @o),  $e);
			$$ = cxt.create(RangeAssign{}, @$, $receiver, r, $assign);
		}
	;

range
	: OPENPAREN[o] expr_or_seq[s] DOTDOT CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, @s, $s);
			$$ = cxt.create(Range{}, range(@o, @c), args, cxt.create(Missing{}, @3));
		}
	| OPENPAREN[o] expr_or_seq[s] DOTDOT expr_or_seq[e] CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, @s, $s);
			$$ = cxt.create(Range{}, range(@o, @c), args, $e);
		}
	| OPENPAREN[o] expr_or_seq[s] COMMA expr_or_seq[n] DOTDOT expr_or_seq[e] CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, range(@s, @n), $s, $n);
			$$ = cxt.create(Range{}, range(@o, @c), args, $e);
		}
	| OPENPAREN[o] expr_or_seq[s] COMMA expr_or_seq[n] DOTDOT CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, range(@s, @n), $s, $n);
			$$ = cxt.create(Range{}, range(@o, @c), args, cxt.create(Missing{}, @5));
		}
	| OPENPAREN[o] DOTDOT expr_or_seq[e] CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, @2);
			$$ = cxt.create(Range{}, range(@o, @c), args, $e);
		}
	;

range.generator
	: OPENPAREN[o] COLON expr_or_seq[s] DOTDOT expr_or_seq[e] CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, @s, $s);
			$$ = cxt.create(RangeGenerator{}, range(@o, @c), args, $e);
		}
	| OPENPAREN[o] COLON expr_or_seq[s] COMMA expr_or_seq[n] DOTDOT expr_or_seq[e] CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, range(@s, @n), $s, $n);
			$$ = cxt.create(RangeGenerator{}, range(@o, @c), args, $e);
		}

	| OPENPAREN[o] COLON expr_or_seq[s] COMMA expr_or_seq[n] DOTDOT  CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, @s, $s, $n);
			$$ = cxt.create(RangeGenerator{}, range(@o, @c), args, cxt.create(Missing{}, @6));
		}
	| OPENPAREN[o] COLON DOTDOT expr_or_seq[e] CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, @3);
			$$ = cxt.create(RangeGenerator{}, range(@o, @c), args, $e);
		}
	| OPENPAREN[o] COLON expr_or_seq[s] DOTDOT CLOSEPAREN[c]
		{
			const auto args = cxt.create(ArgumentList{}, @s, $s);
			$$ = cxt.create(RangeGenerator{}, range(@o, @c), args, cxt.create(Missing{}, @4));
		}
	;


////////////////////////////////////////////////////////////////////////////////
// expression and expression sequences
////////////////////////////////////////////////////////////////////////////////

expr.base	
	: literal 
		{ $$ = $1; }

	| generator 
		{ $$ = $1; }

	| name 
		{ $$ = $1; }

	| CURRYARG 
		 { $$ = cxt.create(CurryNode{}, @1); }

	| msgsend 
		{ $$ = $1; }

	| OPENPAREN expr_or_seq[contents] CLOSEPAREN 
		{ 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr($contents);
			$$ = $contents;
		}

	| OPENPAREN[o] error[e]
		{
			const auto close = error_recovery::bracket(cxt, @o);
			$$ = cxt.create_error(range(@o, close), @e);
			yyclearin; // The lookahead was skipped.
			yyerrok; // Recovered at the matching bracket, so report the next error.
		}

	| TILDE name 
		{ $$ = cxt.create(EnvIdentifierNode{}, @$, $2); }

	| expr.base[receiver] OPENSQUARE arguments[args] CLOSESQUARE
		{ $$ = cxt.create(AtMessage{}, @$, $receiver, $args ); }

	| expr.base[receiver] OPENSQUARE CLOSESQUARE
		{ $$ = cxt.create(AtMessage{}, @$, $receiver, cxt.create(ArgumentList{}, @2)); }

	| expr.base.range 
		{ $$ = $1; }

	| range 
		{ $$ = $1; }

	| range.generator 
		{ $$ = $1; }
	;
	

expr	
	: expr.base { $$ = $1; }
	| expr.base.range.assign { $$ = $1; }
	| expr.dotrange { $$ = $1; }
	| expr.dotrange.assign { $$ = $1; }

	| expr[receiver] DOT OPENSQUARE arguments[args] CLOSESQUARE
		{ $$ = cxt.create(AtMessage{}, @$, $receiver, $args ); }

	| expr[receiver] DOT OPENSQUARE CLOSESQUARE
		{ $$ = cxt.create(AtMessage{}, @$, $receiver, cxt.create(ArgumentList{}, @3) ); }

	| expr[lhs] binary_op[selector] expr[rhs] %prec BINOP 
		{ $$ = cxt.create(BinaryMessage{}, @$, $selector, $lhs, $rhs); }

	| expr DOT name EQUALSSIGN expr
		{ $$ = cxt.create(SetterNode{}, @$, $1, $3, $5); }

	| CLASSNAME { $$ = cxt.create(ClassNameIdentifier{}, @$); }
	| BACKTICK expr { $$ = cxt.create(ReferenceNode{}, @$, $2); }

	| name EQUALSSIGN expr { $$ = cxt.create(AssignmentNode{}, @$, $1, $3); }

	| TILDE name EQUALSSIGN expr
		{ $$ = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, @$, $2, $4); }

	| name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
		{ $$ = cxt.create(SetterNode{}, @$, $3, $1, $6); }

	| name.pack[names] EQUALSSIGN expr[e]
		{ $$ = cxt.create(VariablePackAssign{}, @$, $names, $e); }

	| name.pack[names] OPENPAREN expr_or_seq[expr_seq] CLOSEPAREN
		{ $$ = cxt.create(VariablePackAssign{}, @$, $names, $expr_seq); }

	// This is a slight change from the original. You used to be required to give an argument, even if the 'put' method had a default.
	// Likewise you couldn't use variadic or keywords, even if the 'put' method supported these.
	// These might still be disabled in the compiler stage.
	| expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr 
		{ $$ = cxt.create(AssignmentAtNode{}, @$, $1, $3, $6); }

	| expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr[e] 
		{ $$ = cxt.create(AssignmentAtNode{}, @$, $1, cxt.create(ArgumentList{}, @2), $e); }

	| expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr[e] 
		{ $$ = cxt.create(AssignmentAtNode{}, @$, $1, $4, $e); }

	| expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr[e] 
		{ $$ = cxt.create(AssignmentAtNode{}, @$, $1, cxt.create(ArgumentList{}, @3), $e); }
	;




expr_or_seq.base  
	: expr { $$ = $1; }		
	| expr_or_seq.base SEMICOLON expr { $$ = cxt.create(ExprSeq{}, @$, $1, $3); }
	;

expr_or_seq : 
	expr_or_seq.base semicolon.opt { $$ = $1; }
	;


////////////////////////////////////////////////////////////////////////////////
// generators
////////////////////////////////////////////////////////////////////////////////
generator 
	: OPENCURLY COLON expr_or_seq[expr] COMMA generator.qual.list[list] comma.opt CLOSECURLY
		{ $$ = cxt.create(GeneratorNode{}, @$, $expr, $list); }

	| OPENCURLY SEMICOLON expr_or_seq[expr] COMMA generator.qual.list[list] comma.opt CLOSECURLY
		{ $$ = cxt.create(GeneratorFakeNode{}, @$, $expr, $list); }
	;

generator.qual.list
	: generator.qual[item]
		{ $$ = cxt.create(GeneratorQualList{}, @$, $item); }
	| generator.qual.list[list] COMMA generator.qual[item]
		{ $$ = cxt.graph->append($list, @$, $item); }
	;

generator.qual
	: name[elem] LEFTARROW expr_or_seq[expr]
		{ $$ = cxt.create(GeneratorQualAssign{}, @$, $elem, cxt.create(Missing{}, @2), $expr); }
	| name[elem] name[index] LEFTARROW expr_or_seq[expr]
		{ $$ = cxt.create(GeneratorQualAssign{}, @$, $elem, $index, $expr); }
	| VAR name EQUALSSIGN expr_or_seq[expr]
		{ $$ = cxt.create(GeneratorQualDeclare{}, @$, $name, $expr); }
	| VAR name OPENPAREN expr_or_seq[expr] CLOSEPAREN
		{ $$ = cxt.create(GeneratorQualDeclare{}, @$, $name, $expr); }
	| expr_or_seq[expr]
		{ $$ = cxt.create(GeneratorQualIf{}, @$, $expr); }
	| COLON COLON expr_or_seq[expr] 
		{ $$ = cxt.create(GeneratorQualSideEffect{}, @$, $expr); }
	| COLON WHILE expr_or_seq[expr]
		{ $$ = cxt.create(GeneratorQualGuard{}, @$, $expr); }
	;


////////////////////////////////////////////////////////////////////////////////
// arguments
////////////////////////////////////////////////////////////////////////////////
// Everything is duplicated because pipe lists and arg lists don't acceept the same things.
// Additionally, each default records whether it is bracketed, which decides whether passing nil keeps nil.

// This can't be refactored into an 'item' and a 'list', it creates some conflicts.
argument_declarations.list 
	: name 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, $1); }
	| name EQUALSSIGN expr 
		{ 
			const auto is_bracketed = cxt.graph->is_bracketed($expr);
			$$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, @$, $name, $expr)); 
		}
	| name OPENPAREN expr_or_seq CLOSEPAREN 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $name, $expr_or_seq)); }

	| argument_declarations.list[list] COMMA name 
		{ $$ = cxt.graph->append( $1, @$, $3); }

	| argument_declarations.list[list] COMMA name EQUALSSIGN expr 
		{ 
			const auto is_bracketed = cxt.graph->is_bracketed($expr);
			$$ = cxt.graph->append( $list, @$, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, range(@name, @expr), $name, $expr));
		}
	| argument_declarations.list[list] COMMA name OPENPAREN expr_or_seq CLOSEPAREN
		{ $$ = cxt.graph->append($list, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, range(@name, @6), $name, $expr_or_seq)); }

	;

argument_declarations.pipelist 
	: name literal
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $1, $2)); }
	| name 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, $1); }
	| name EQUALSSIGN literal 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $1, $3)); }
	| name OPENPAREN expr_or_seq CLOSEPAREN 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $1, $3)); }
	| name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $1, $4)); }
	| argument_declarations.pipelist comma.opt name literal
		{ $$ = cxt.graph->append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, range(@3, @4), $3, $4)); }
	| argument_declarations.pipelist comma.opt name 
		{ $$ = cxt.graph->append( $1, @$, $3); }
	| argument_declarations.pipelist comma.opt name EQUALSSIGN literal
		{ $$ = cxt.graph->append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, range(@3, @5), $3, $5)); }
	| argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
		{ $$ = cxt.graph->append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, range(@3, @6), $3, $5)); }
	| argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
		{ $$ = cxt.graph->append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, range(@3, @7), $3, $6)); }
	;

argument_declarations
	: ARG SEMICOLON { $$ = cxt.create(DeclareArgumentList{}, @$); }
	| ARG argument_declarations.list comma.opt SEMICOLON { $$ = $2; }
	| ARG argument_declarations.list ELLIPSIS name SEMICOLON  
		{ $$ = cxt.graph->append( $2, @$, cxt.create(DeclareArgumentVariadicNode{}, @4, $4)); }
	| ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON 
		{ $$ = cxt.graph->append( $2, @$, cxt.create(DeclareArgumentVariadicNode{}, @4, $4), cxt.create(DeclareArgumentVariadicNode{}, @6, $6)); }
	| ARG ELLIPSIS name[args] SEMICOLON 
		{ 
			auto arg_list = cxt.create(DeclareArgumentList{}, @$);
			$$ = cxt.graph->append(arg_list, @$, cxt.create(DeclareArgumentVariadicNode{}, @args, $args)); 
		}
	| ARG ELLIPSIS name[args] COMMA name[vars] SEMICOLON 
		{
			auto arg_list = cxt.create(DeclareArgumentList{}, @$);
			$$ = cxt.graph->append(
				arg_list, @$,
				cxt.create(DeclareArgumentVariadicNode{}, @args, $args),
				cxt.create(DeclareArgumentVariadicNode{}, @vars, $vars)
			); 
		}
	| PIPE PIPE 
		{ $$ = cxt.create(DeclareArgumentList{}, @$); }
	| PIPE argument_declarations.pipelist comma.opt PIPE 
		{ $$ = $2; }
	| PIPE argument_declarations.pipelist ELLIPSIS name PIPE 
		{ $$ = cxt.graph->append($2, @2, cxt.create(DeclareArgumentVariadicNode{}, @4, $4)); }
	| PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE 
		{ $$ = cxt.graph->append($2, @$, cxt.create(DeclareArgumentVariadicNode{}, @4, $4), cxt.create(DeclareArgumentVariadicNode{}, @6, $6)); }
	| PIPE ELLIPSIS name[args] PIPE 
		{ 
			auto arg_list = cxt.create(DeclareArgumentList{}, @$);
			$$ = cxt.graph->append(arg_list, @$, cxt.create(DeclareArgumentVariadicNode{}, @args, $args)); 
		}
	| PIPE ELLIPSIS name[args] COMMA name[vars] PIPE 
		{
			auto arg_list = cxt.create(DeclareArgumentList{}, @$);
			$$ = cxt.graph->append(
				arg_list, @$,
				cxt.create(DeclareArgumentVariadicNode{}, @args, $args),
				cxt.create(DeclareArgumentVariadicNode{}, @vars, $vars)
			); 
		}
	;

argument_declarations.opt
	: %empty { $$ = cxt.create(DeclareArgumentList{}, @$); }
	| argument_declarations { $$ = $1; }
	;


arguments.entries 	
	: KEYBINOP expr_or_seq { $$ = cxt.create(KwArgNode{}, @$, cxt.create(KeyBinOp{}, @1), $2); }
	| MULTIPLY expr_or_seq { $$ = cxt.create(VariadicArgNode{}, @$, $2); }
	| expr_or_seq { $$ = $1; }
	;

arguments.no_trailing
	: arguments.entries {  $$ = cxt.create(ArgumentList{}, @$, $1); }
	| arguments.no_trailing COMMA arguments.entries { $$ = cxt.graph->append($1, @$, $3); }
	;

arguments : arguments.no_trailing comma.opt { $$ = $1; };

arguments.paren : OPENPAREN arguments CLOSEPAREN { $$ = $2; }

arguments.maybe_paren 
	: %empty { $$ = cxt.create(ArgumentList{}, @$); }
	| OPENPAREN CLOSEPAREN
		{ $$ = cxt.create(ArgumentList{}, @$); }
	| OPENPAREN arguments CLOSEPAREN
		{ $$ = $arguments; }
	;

////////////////////////////////////////////////////////////////////////////////
// variable declarations
////////////////////////////////////////////////////////////////////////////////

variable_declarations.list.item 
	: name 
		{ $$ = $1; }
	| name EQUALSSIGN expr
		{ $$ = cxt.create(DeclareVariableWithDefaultNode{}, @$, $1, $3); }
	| name OPENPAREN expr_or_seq CLOSEPAREN
		{
			// Bracketed, as in the old compiler, so it isn't a literal, see ConstEval::is_legacy_literal.
			cxt.graph->register_bracketed_expr($3);
			$$ = cxt.create(DeclareVariableWithDefaultNode{}, @$, $1, $3);
		}
	| name.pack[names] EQUALSSIGN expr
		{ $$ = cxt.create(VariablePackAssign{}, @$, $names, $expr); }
	| name.pack[names] OPENPAREN expr_or_seq[expr] CLOSEPAREN
		{
			cxt.graph->register_bracketed_expr($expr);
			$$ = cxt.create(VariablePackAssign{}, @$, $names, $expr);
		}
	;

variable_declarations.list
	: variable_declarations.list.item
		{ $$ = cxt.create(DeclareVariableList{}, @$, $1); }
	| variable_declarations.list COMMA variable_declarations.list.item
		{$$ = cxt.graph->append($1, $3); }
	;

variable_declarations : VAR variable_declarations.list { $$ = $2; };


////////////////////////////////////////////////////////////////////////////////
// literals
////////////////////////////////////////////////////////////////////////////////

literal.terminal 	
	: symbol { $$ = $1; }
	| string { $$ = $1; }
	| integer { $$ = $1; }
	| float { $$ = $1; }
	| boolean { $$ = $1; }
	| nil { $$ = $1; }
	| ascii { $$ = $1; }
	| block { $$ = $1; }
	;

literal.array.contents 
	: %empty 
		{ $$ = cxt.create(ArrayNode{}, @$); }
	| expr_or_seq 
		{ $$ = cxt.create(ArrayNode{}, @$, $1); }
	| expr_or_seq COLON expr_or_seq
		{ $$ = cxt.create(ArrayNode{}, @$, $1, $3); }
	| KEYBINOP expr_or_seq
		{ $$ = cxt.create(ArrayNode{}, @$, cxt.create(KeyBinOp{}, @1), $2); }
	| literal.array.contents COMMA expr_or_seq
		{ $$ = cxt.graph->append($1, @$, $3); }
	| literal.array.contents COMMA expr_or_seq COLON expr_or_seq
		{ $$ = cxt.graph->append($1, @$, $3, $5); }
	| literal.array.contents COMMA KEYBINOP expr_or_seq
		{ $$ = cxt.graph->append($1, @$, cxt.create(KeyBinOp{}, @3), $4); }
	;

literal.dictionary.entry 
	// This is a breaking change!
	// Previously you could have ( 1;2;3;4;5;6; : 10). now the expr_or_seq is invalid, but you can have ((1;2;3;4): 10)
	// This is necessary because of the scd format as (1;2;3;4;5;6;7;) is a valid file, and the parser would need infinite look ahead to figure out which is which.
	: expr COLON expr_or_seq 
		{ $$ = cxt.create(DictionaryEntryNode{}, @$, $1, $3); }
	| KEYBINOP expr_or_seq 
		{ $$ = cxt.create(DictionaryEntryNode{}, @$, cxt.create(KeyBinOp{}, @1), $2); }
	;

literal.dictionary.entries 
	: %empty 
		{ $$ = cxt.create(DictionaryNode{}, @$); }
	| literal.dictionary.entry 
		{ $$ = cxt.create(DictionaryNode{}, @$, $1); }
	| literal.dictionary.entries COMMA literal.dictionary.entry
		{ $$ = cxt.graph->append($1, @$,  $3); }
	;

literal.dictionary 
	: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN 
		{ cxt.graph->location(*$2) = @$; $$ = $2; }
	;

literal.array	
	: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE 
		{  cxt.graph->location(*$2) = @$; $$ = $2; }	
	| HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE 
		{  
			cxt.graph->payload($3).is_immutable = true;
			cxt.graph->location(*$3) = @$;
			$$ = $3;
		}	
	;
				
literal	
	: literal.terminal { $$ = $1; }
	| literal.array { $$ = $1; }
	| literal.dictionary { $$ = $1; }
	;

////////////////////////////////////////////////////////////////////////////////
// Name terminal and name list
////////////////////////////////////////////////////////////////////////////////

name 
	: NAME { $$ = cxt.create(NamedIdentifier{}, @$); } 
	| WHILE { $$ = cxt.create(NamedIdentifier{}, @$); }
	;

// Used for destructuring
name.list 
	: HASH name
		{ $$ = cxt.create(NameList{}, @$, $name); }
	| HASH TILDE name 
		{ $$ = cxt.create(NameList{}, @$, cxt.create(EnvIdentifierNode{}, range(@2, @3), $name)); }
	| name.list[list] COMMA name
		{ $$ = cxt.graph->append($list, @$, $name); }
	| name.list[list] COMMA TILDE name
		{ $$ = cxt.graph->append($list, @$, cxt.create(EnvIdentifierNode{}, range(@3, @4), $name)); }
	;
name.pack 
	: name.list[list]
		{ $$ = cxt.create(NamePack{}, @$, $list, cxt.create(Missing{}, @list)); }
	| name.list[list] ELLIPSIS name
		{ $$ = cxt.create(NamePack{}, @$, $list, $name); }
	| name.list[list] ELLIPSIS TILDE[t] name
		{ $$ = cxt.create(NamePack{}, @$, $list, cxt.create(EnvIdentifierNode{}, range(@t, @name), $name)); }
	;


////////////////////////////////////////////////////////////////////////////////
// Other terminals and 'literals'
////////////////////////////////////////////////////////////////////////////////



ascii : ASCII { $$ = cxt.create(ASCIINode{}, @$); } ;

nil : NIL { $$ = cxt.create(NilNode{}, @$); } ;

boolean	
	: TRUE { $$ = cxt.create(BooleanNode{true}, @$); }
	| FALSE { $$ = cxt.create(BooleanNode{false}, @$); }
	;

symbol 	
	: SYMBOL_QUOTE { $$ = cxt.create(SymbolQuote{}, @$); }
	| SYMBOL_SLASH { $$ = cxt.create(SymbolSlash{}, @$); }
	;

string 	
	: STRINGLINE { $$ = cxt.create(StringLineList{}, @$, cxt.create(StringLineNode{}, @$)); }
	| string STRINGLINE { $$ = cxt.graph->append($1, cxt.create(StringLineNode{}, @2)); }
	;

integer	
	: INTEGER { $$ = cxt.create(IntNode{}, @$); }
	| INTEGER_RADIX { $$ = cxt.create(IntNode{IntNode::Kind::Radix}, @$); }
	| HEXADECIMAL { $$ = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, @$); }
	| MINUS integer %prec UMINUS 
		{
			// Reaches into the previous integer and changes its sign.
			cxt.graph->payload($2).sign = IntNode::Sign::Negative;
			$$ = $2;
		}
	;

float.raw_unsigned 	
	: FLOAT { $$ = cxt.create(FloatNode{}, @$); }
	| FLOAT_RADIX { $$ = cxt.create(FloatNode{FloatNode::Kind::Radix}, @$); }
	| FLOAT_EXPONENT { $$ = cxt.create(FloatNode{FloatNode::Kind::Exponent}, @$); }
	| FLOAT_INF { $$ = cxt.create(FloatNode{FloatNode::Kind::Inf}, @$); }
	;

float.raw	
	: float.raw_unsigned 
		{ $$ = $1; }
	| MINUS float.raw_unsigned %prec UMINUS 
		{ cxt.graph->payload($2).sign = FloatNode::Sign::Negative; $$ = $2; }
	;

accidental.unsigned 
	: ACCIDENTAL_STEPS 
		{ $$ = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, @$); }
	| ACCIDENTAL_CENTS 
		{ $$ = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, @$); }
	;

accidental  
	: accidental.unsigned 
		{ $$ = $1; }
	| MINUS accidental.unsigned %prec UMINUS 
		{ cxt.graph->payload($2).sign = AccidentalNode::Sign::Negative; $$ = $2; }
	;

float	
	: float.raw { $$ = $1; }
	| accidental { $$ = $1; }
	| float.raw PI { $$ = cxt.create(PiNode{}, @$, $1); }
	| integer PI { $$ = cxt.create(PiNode{}, @$, $1); }
	| PI  { $$ = cxt.create(PiNode{}, @$, cxt.create(Missing{}, @$)); }
	| MINUS PI { $$ = cxt.create(PiNode{PiNode::Sign::Negative}, @$, cxt.create(Missing{}, @$)); }
	;

accessor
	: %empty { $$ = cxt.create(RWAccessor{ReadWriteAccessor::Private}, @$); }
	| LESSTHAN { $$ = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, @$); }
	| GREATERTHAN { $$ = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, @$); }
	| READWRITEVAR 
		{ 
			$$ = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, @$); 
		}
	;

////////////////////////////////////////////////////////////////////////////////
// Misc
////////////////////////////////////////////////////////////////////////////////

semicolon.opt : %empty | SEMICOLON;
comma.opt : %empty | COMMA;

%%
