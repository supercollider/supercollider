// Copyright Jordan Henderson 2026
%require "3.8"
%language "C++"

%define api.value.type variant
%define api.token.prefix {TOKEN_}
%define api.namespace {sc::ast::parser}
%define parse.error custom

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

#include "sc_lexer/lexer.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/nodes.hpp"

#include "sc_grammar_parser.hpp"
#include "sc_grammar_impl.hpp"
#include "parser_context.hpp"


using namespace sc::ast;

template<typename... REJECTS>
auto create_error(sc::ast::parser::ParserContext& cxt, sc::lex::SourceCodeRange loc, REJECTS...rejects) {
	const auto orphans = cxt.graph.orphans();
	auto er = cxt.create(Error{}, loc);
	for(auto o : orphans){
		if (!((*o == *rejects) || ...))
			cxt.graph.append(er, sc::ast::AnyIndex{*o});
	}
	return er;
}

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

%type <ClassListOrExprListIndex> go

%type <RegionListIndex> region
%type <error_index<AnyExprIndex>> region.item 

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
%type <AnyMethodIndex> method 
%type <MethodIndex> method.base
%type <MethodListIndex> method.list method.list.opt

%type<error_index<AnyExprIndex>> expr.error

%type <AnyExprIndex> msgsend // not all msgsends result in a message node!

%type<ArgumentEntryIndex> arguments.entries
%type<ArgumentListIndex> arguments arguments.no_trailing arguments.paren arguments.maybe_paren

%type <DeclareArgumentListIndex> argument_declarations.list argument_declarations argument_declarations.pipelist argument_declarations.opt

%type <DeclareVariableListIndex> variable_declarations.list variable_declarations
%type <DeclareAnyVariableIndex> variable_declarations.list.item

%type<BlockListIndex> block.opt_list block.list

%type <AnyExprIndex> expr expr.base
%type <AnyExprIndex> expr.seq expr.seq.base 

%type <NamedIdentifierIndex> name 

%type <SelectorIndex> binary_op.no_adverb binary_op.raw
%type <SelectorMaybeAdverbIndex> binary_op
%type <AdverbIndex> adverb

// literals
%type <AnyPossiblyLiteralIndex> literal literal.terminal 

%type <ArrayIndex> literal.array literal.array.contents
%type <DictionaryIndex> literal.dictionary literal.dictionary.entries
%type <DictionaryEntryIndex> literal.dictionary.entry

%type <NameListIndex> name.list

%type <BlockItemIndex> block.contents.item
%type <BlockContentsListIndex> block.contents 
%type <BlockIndex> block 
%type <NilLitIndex> nil
%type <BooleanLitIndex> boolean
%type <SymbolLitIndex> symbol
%type <StringLitIndex> string
%type <IntLitIndex> integer
%type <FloatLitIndex> float.raw float.raw_unsigned
%type <AccidentalIndex> accidental accidental.unsigned
%type <FloatProducingIndex> float
%type <ASCIIIndex> ascii

// misc
%type <RWAccessorIndex> accessor 


////////////////////////////////////////////////////////////////////////////////
// start rule
////////////////////////////////////////////////////////////////////////////////

%start go


////////////////////////////////////////////////////////////////////////////////
// rules
////////////////////////////////////////////////////////////////////////////////

%%

go 
	: region semicolon.opt YYEOF { $$ = cxt.graph.assign_root($region); }
	| classOrExtList.list[classes] YYEOF  { $$ = cxt.graph.assign_root($classes); }
	| %empty { $$ = cxt.graph.assign_root(cxt.create(ClassOrExtensionList{}, @$));}
	;

////////////////////////////////////////////////////////////////////////////////
// region
////////////////////////////////////////////////////////////////////////////////

region.item
	: expr
		{ $$ = $expr; } 
	| OPENPAREN argument_declarations[args] block.contents[content] CLOSEPAREN 
		{ 
			auto block = cxt.create(BlockNode{}, @$, $args, $content); 

			$$ = cxt.create(
				MessageNode{MessageNode::SelectorMode::Value}, 
				@$, 
				cxt.create(Missing{}, @$),
				cxt.create(ArgumentList{}, @$, block)
			);
		}
	| OPENPAREN argument_declarations[args] CLOSEPAREN 
		{ 
			auto block = cxt.create(BlockNode{}, @$, $args, cxt.create(BlockContentsList{}, @$));

			$$ = cxt.create(
				MessageNode{MessageNode::SelectorMode::Value}, 
				@$, 
				cxt.create(Missing{}, @$),
				cxt.create(ArgumentList{}, @$, block)
			);
		}
	;

region
	: 
	INTERPRET expr
		{ $$ = cxt.create(RegionList{}, @$, $expr); } 
	| INTERPRET error
		{
			error_recovery::expr(cxt);
			yyclearin;
			$$ = cxt.create(RegionList{}, @$, create_error(cxt, @error));
		}
	| INTERPRET OPENPAREN argument_declarations[args] block.contents[content] semicolon.opt CLOSEPAREN 
		{ 
			auto block = cxt.create(BlockNode{}, @$, $args, $content); 

			auto msg = cxt.create(
				MessageNode{MessageNode::SelectorMode::Value}, 
				@$, 
				cxt.create(Missing{}, @$),
				cxt.create(ArgumentList{}, @$, block)
			);
			$$ = cxt.create(RegionList{}, @$, msg);
		}
	| INTERPRET OPENPAREN argument_declarations[args] CLOSEPAREN 
		{ 
			auto block = cxt.create(BlockNode{}, @$, $args, cxt.create(BlockContentsList{}, @$));

			auto msg = cxt.create(
				MessageNode{MessageNode::SelectorMode::Value}, 
				@$, 
				cxt.create(Missing{}, @$),
				cxt.create(ArgumentList{}, @$, block)
			);

			$$ = cxt.create(RegionList{}, @$, msg);
		}

	| region[r] SEMICOLON region.item[item]
		{ $$ = cxt.graph.append($r, @$, $item); }

	| region[r] REGION_SEPARATOR region.item[item]
		{ $$ = cxt.graph.append($r, @$, $item); }

	| region[r] error[e] 
		{
			if (@r.end.line_number != @e.begin.line_number){
				auto first_child = cxt.graph.edges(*$r).first_child.value();
				auto last_child = cxt.graph.last_child(*$r);
				auto loc = cxt.graph.location(last_child ? Index{*last_child} : Index{first_child});
				error_recovery::region_separator(cxt, loc);
				cxt.region_recovery = sc::ast::parser::ParserContext::RegionRecovery::EmitRegionSeparator;
				static_assert(std::is_same_v<decltype(yyerrstatus_), int>);
				yyerrstatus_ = 0; // this is NOT in the api, but the only way to get errors to re-emit.
				yyclearin;
				$$ = $1;
			} else {
				error_recovery::expr(cxt);
				$$ = cxt.graph.append($r, create_error(cxt, @e, $r));
				yyclearin;
			} 
		}
	;

expr.error
	: expr { $$ = $expr; }

	;



////////////////////////////////////////////////////////////////////////////////
// class or class extension list
////////////////////////////////////////////////////////////////////////////////

classOrExtList.list
	: classOrExtList.item[item]
		{ $$ = cxt.create(ClassOrExtensionList{}, @$, $item); }
	| classOrExtList.list[list] classOrExtList.item[item]
		{ $$ = cxt.graph.append($list, $item); }
	;

classOrExtList.item
	: class { $$ = $1; }
	| class.extension { $$ = $1; }
	;

class 
	: CLASSNAME[name] class.slot.opt[slot] class.super.opt[super] OPENCURLY members.opt[vars] method.list.opt[meths] CLOSECURLY
		{ $$ = cxt.create(Class{}, @$, cxt.create(ClassNameIdentifier{}, @name), $slot, $super, $vars, $meths); }
	;

class.super.opt 
	: %empty { $$ = cxt.create(Missing{}, @$); } 
	| COLON CLASSNAME { $$ = cxt.create(ClassNameIdentifier{}, @CLASSNAME); };
	;

class.slot.opt 
	: %empty { $$ = cxt.create(Missing{}, @$); } 
	| OPENSQUARE name CLOSESQUARE { $$ = $name; };
	;

class.extension 
	: ADD CLASSNAME[name] OPENCURLY method.list.opt[meths] CLOSECURLY
		{ $$ = cxt.create(ClassExtension{}, @$, cxt.create(ClassNameIdentifier{}, @name), $meths); }
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
		{ $$ = cxt.graph.append($1, $3); }
	;

members.entry
	: CLASSVAR members.entry.list[vars]
		{ $$ = cxt.graph.cast<DeclareClassMemberList>($vars); }
	| VAR members.entry.list[vars]
		{ $$ = cxt.graph.cast<DeclareMemberList>($vars); }
	| CONST members.entry.list[vars]
		{ $$ = cxt.graph.cast<DeclareConstList>($vars); }
	;

members
	: members.entry 
		{ $$ = cxt.create(BigMemberList{}, @$, $1);}
	| members SEMICOLON members.entry
		{ $$ = cxt.graph.append($1, $3); }
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
		{ $$ = cxt.create(Method{}, @$, $name, $args, cxt.create(Missing{},@2), cxt.create(BlockList{}, @4)); }
	| method.name[name] OPENCURLY argument_declarations.opt[args] PRIMITIVENAME[prim] semicolon.opt block.contents[block] semicolon.opt CLOSECURLY
		{ $$ = cxt.create(Method{}, @$, $name, $args, cxt.create(PrimitiveIdentifier{}, @prim), $block); }
	| method.name[name] OPENCURLY argument_declarations.opt[args] PRIMITIVENAME[prim] semicolon.opt CLOSECURLY
		{ $$ = cxt.create(Method{}, @$, $name, $args, cxt.create(PrimitiveIdentifier{}, @prim), cxt.create(BlockList{}, @5)); }
	;

method
	: method.base { $$ = $1; }
	| MULTIPLY method.base[meth] 
		{ $$ = cxt.graph.cast<ClassMethod>($meth); }
	;

method.list 		
	: method { $$ = cxt.create(MethodList{}, @$, $1); }
	| method.list method { $$ = cxt.graph.append($1, @$, $2); }
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
	: %empty { $$ = {}; } 
	| block.list { $$ = $1; } 
	;

block.list 		
	: block { $$ = cxt.create(BlockList{}, @$, $1); }
	| block.list block { $$ = cxt.graph.append($1, @$, $2); }
	;

block.contents
	: block.contents.item { $$ = cxt.create(BlockContentsList{}, @$, $1); }
	| block.contents SEMICOLON block.contents.item { $$ = cxt.graph.append($1, @$, $3); }
	| block.contents NONLOCALRETURN expr 
		{ 
			auto non_local = cxt.create(NonLocalReturnExpr{}, @expr, $expr);
			$$ = cxt.graph.append($1, @$, non_local); 
		}
	| block.contents SEMICOLON NONLOCALRETURN expr 
		{ 
			auto non_local = cxt.create(NonLocalReturnExpr{}, @expr, $expr);
			$$ = cxt.graph.append($1, @$, non_local); 
		}
	| NONLOCALRETURN expr 
		{ 
			auto non_local = cxt.create(NonLocalReturnExpr{}, @expr, $expr);
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
	| DOT OPENPAREN expr.seq CLOSEPAREN { $$ = cxt.create(AdverbExprNode{}, @$, $3);  };
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
		{
			if ($blocks) {
				cxt.graph.merge($args, $blocks);
				cxt.graph.location(*$args) = @$; // spans arguments and block list
			}
			$$ = cxt.create(MessageNode{}, @$, $selector, $args);
		}

	| OPENPAREN binary_op.no_adverb[args] CLOSEPAREN OPENPAREN CLOSEPAREN block.list[blocks]
		{ $$ = cxt.create(MessageNode{}, @$, $args, $blocks); }

	| OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
		{ $$ = cxt.create(MessageNode{}, @$, $2, cxt.create(ArgumentList{}, @$, $4)); }


	| name OPENPAREN arguments CLOSEPAREN block.opt_list
		{ 
			if($5){
				cxt.graph.merge($3, $5);
				cxt.graph.location(*$3) = {@3.begin, @5.end}; // spans arguments and block list
			}
			$$ = cxt.create(MessageNode{}, @$, $1, $3); 
		}

	| name OPENPAREN CLOSEPAREN block.list[blocks]
		{ $$ = cxt.create(MessageNode{}, @$, $1, $blocks); }

	| name block.list 
		{ $$ = cxt.create(MessageNode{}, @$, $1, cxt.create(ArgumentList{}, @2, $2)); }
	
	| expr DOT name arguments.maybe_paren block.opt_list
		{ 
			if ($5) {
				cxt.graph.merge($4, $5);
				cxt.graph.location(*$4) = {@4.begin, @5.end}; // spans arguments and block list
			}
			cxt.graph.prepend($4, $1); // put the receiver in place
			cxt.graph.location(*$4) = {@1.begin, @4.end}; // spans arguments and block list
			$$ = cxt.create(MessageNode{}, @$, $3, $4);
		}

	| expr DOT arguments.paren block.opt_list
		{ 
			if ($4) {
				cxt.graph.merge($3, $4);
				cxt.graph.location(*$3) = {@3.begin, @4.end}; // spans arguments and block list
			}
			cxt.graph.prepend($3, $1); // put the receiver in place
			cxt.graph.location(*$3) = {@1.begin, @3.end}; // spans arguments and block list
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::Value}, @$, cxt.create(Missing{}, @2), $3);
		}
	| expr[rec] DOT OPENPAREN CLOSEPAREN block.opt_list[blocks]
		{
			auto args = cxt.create(ArgumentList{}, @$, $rec);
			if ($blocks) cxt.graph.merge(args, $blocks);
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::Value}, @$, cxt.create(Missing{}, @2), args);
		}
		

//	| expr DOT error 
//		{
//			auto unexpected = cxt.consume_error(); 
//			std::cout << "GOT AN ERROR WITH A DOT"<< std::endl;
//			$$ = $1;
//		}

	| CLASSNAME OPENSQUARE literal.array.contents CLOSESQUARE
		{  $$ = cxt.create(CollectionNode{}, @$, cxt.create(ClassNameIdentifier{}, @1), $3); }
	
	| CLASSNAME block.list
		{
			auto args = cxt.create(ArgumentList{}, @$, cxt.create(NamedIdentifier{}, @1));
			cxt.graph.merge(args, $2);
			$$ = cxt.create(MessageNode{}, @$, cxt.create(Missing{}, @1), args);
		}
	| CLASSNAME OPENPAREN arguments[args] CLOSEPAREN block.opt_list[blocks]
		{
			if ($blocks) {
				cxt.graph.merge($args, $blocks);
				cxt.graph.location(*$args) = {@args.begin, @blocks.end}; // spans arguments and block list
			}
			cxt.graph.prepend($args, cxt.create(ClassNameIdentifier{}, @1)); // put the receiver in place
			cxt.graph.location(*$args) = {@args.begin, @blocks.end}; // spans arguments and block list
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::New}, @$, cxt.create(Missing{}, @1), $args);
		}

	| CLASSNAME OPENPAREN CLOSEPAREN block.opt_list[blocks]
		{
			auto args = cxt.create(ArgumentList{}, @$, $blocks);
			cxt.graph.prepend(args, cxt.create(ClassNameIdentifier{}, @1)); // put the receiver in place
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::New}, @$, cxt.create(Missing{}, @1), args);
		}
	;

////////////////////////////////////////////////////////////////////////////////
// expression and expression sequences
////////////////////////////////////////////////////////////////////////////////

expr.base	
	: literal { $$ = $1; }
	| generator { $$ = $1; }
	| name { $$ = $1; }
	// | curry_arg { $$ = $1; }
	| msgsend { $$ = $1; }
	| OPENPAREN block.contents[contents] semicolon.opt CLOSEPAREN 
		{ 
			auto blk = cxt.create(BlockNode{}, @$, cxt.create(DeclareArgumentList{}, @$), $contents);
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::Value}, @$.flatten(), cxt.create(Missing{}, @$.flatten()), cxt.create(ArgumentList{}, @$, blk));
		}
	| TILDE name { $$ = cxt.create(EnvIdentifierNode{}, @$, $2); }

	// | OPENPAREN valrange2 CLOSEPAREN
	// | OPENPAREN COLON valrange3 CLOSEPAREN

	| expr.base OPENSQUARE arguments CLOSESQUARE
		{ 
			cxt.graph.prepend($3, $1); // put receiver in place.
			cxt.graph.location(*$3) = @$;
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::At}, @$, cxt.create(Missing{}, @2), $3); 
		}

	| expr.base OPENSQUARE CLOSESQUARE
		{ 
			auto args = cxt.create(ArgumentList{}, @$, $1);
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::At}, @$, cxt.create(Missing{}, @2), args); 
		}

	// | valrangex1;
	;

expr	
	: expr.base { $$ = $1; }
	// | valrangexd
	// | valrangeassign

	| CLASSNAME { $$ = cxt.create(ClassNameIdentifier{}, @$); }

	| expr DOT OPENSQUARE arguments CLOSESQUARE
		{ 
			cxt.graph.prepend($4, $1); // put receiver in place
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::At}, @$, cxt.create(Missing{}, @$), $4);
		} 
	| expr DOT OPENSQUARE CLOSESQUARE
		{ 
			auto args = cxt.create(ArgumentList{}, @$, $1);
			$$ = cxt.create(MessageNode{MessageNode::SelectorMode::At}, @$, cxt.create(Missing{}, @3), args);
		} 

	| BACKTICK expr { $$ = cxt.create(ReferenceNode{}, @$, $2); }

	| expr binary_op expr %prec BINOP 
		{ 
			auto args = cxt.create(ArgumentList{}, @$, $1, $3);
			$$ = cxt.create(MessageNode{}, @$, $2, args);
		}

	| name EQUALSSIGN expr
		{ $$ = cxt.create(AssignmentNode{}, @$, $1, $3); }

	| TILDE name EQUALSSIGN expr
		{ $$ = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, @$, $2, $4); }

	| expr DOT name EQUALSSIGN expr
		{ $$ = cxt.create(SetterNode{}, @$, $1, $3, $5); }

	| name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
		{ $$ = cxt.create(SetterNode{}, @$, $3, $1, $6); }

	| HASH name.list[names] comma.opt EQUALSSIGN expr[e]
		{ $$ = cxt.create(VariablePack{}, @$, $names, $e); }
	| HASH name.list[names] comma.opt OPENPAREN expr.seq[expr_seq] CLOSEPAREN
		{ $$ = cxt.create(VariablePack{}, @$, $names, $expr_seq); }

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

expr.seq.base  
	: expr { $$ = $1; }		
	| expr.seq.base SEMICOLON expr 
		{
			// This piece of logic is here because exprs can contain expr.seq, so we avoid creating the list node if we can.
			if(auto expr_seq = cxt.graph.as_a<ExprSeqIndex>(*$1)) {
				cxt.graph.append(*expr_seq, @$, $3);
				$$ = $1;
			} else {
				$$ = cxt.create(ExprSeq{}, @$, $1, $3);
			}
		}
	;

expr.seq : expr.seq.base semicolon.opt { $$ = $1; };


////////////////////////////////////////////////////////////////////////////////
// generators
////////////////////////////////////////////////////////////////////////////////
generator 
	: OPENCURLY COLON expr.seq COMMA generator.qual.list comma.opt CLOSECURLY
		{}
	| OPENCURLY SEMICOLON expr.seq COMMA generator.qual.list comma.opt CLOSECURLY
		{}
	;

generator.next_qual
	: %empty {}
	| COMMA generator.qual {}
	;

generator.qual.list
	: generator.qual[item]
		{ $$ = cxt.create(GeneratorQualList{}, @$, $item); }
	| generator.qual.list[list] COMMA generator.qual[item]
		{ $$ = cxt.graph.append($list, @list, $item); }
	;

generator.qual
	//	Assignment
	: name[elem] LEFTARROW expr.seq[expr]  generator.next_qual[next]
		{}
	// Assignment 2
	| name[elem] name[count] LEFTARROW expr.seq[expr] generator.next_qual[next]
		{}
	// Declare 1
	| VAR name[name] EQUALSSIGN expr.seq[expr] generator.next_qual[next]
		{}
	// Declare 2
	| VAR name OPENPAREN expr.seq CLOSEPAREN generator.next_qual
		{}
	// If
	| expr.seq generator.next_qual
		{}
	// Side effect
	| COLON COLON expr.seq generator.next_qual
		{}
	| COLON WHILE expr.seq generator.next_qual
		{}
	;


////////////////////////////////////////////////////////////////////////////////
// arguments
////////////////////////////////////////////////////////////////////////////////
// Everything is duplicated because pipe lists and arg lists don't acceept the same things.
// Additionally, there is difference between default arguments that preserve nil and not on explicit assignment.

// This can't be refactored into an 'item' and a 'list', it creates some conflicts.
argument_declarations.list 
	: name 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, $1); }
	| name EQUALSSIGN literal 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $1, $3)); }
	| name OPENPAREN expr.seq CLOSEPAREN 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $1, $3)); }
	| name EQUALSSIGN OPENPAREN expr.seq CLOSEPAREN 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $1, $4)); }
	| argument_declarations.list COMMA name 
		{ $$ = cxt.graph.append( $1, @$, $3); }
	| argument_declarations.list COMMA name EQUALSSIGN literal 
		{ $$ = cxt.graph.append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $3, $5)); }
	| argument_declarations.list COMMA name OPENPAREN expr.seq CLOSEPAREN
		{ $$ = cxt.graph.append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $3, $5)); }
	| argument_declarations.list COMMA name EQUALSSIGN OPENPAREN expr.seq CLOSEPAREN
		{ $$ = cxt.graph.append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $3, $6)); }
	;

argument_declarations.pipelist 
	: name literal
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $1, $2)); }
	| name 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, $1); }
	| name EQUALSSIGN literal 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $1, $3)); }
	| name OPENPAREN expr.seq CLOSEPAREN 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $1, $3)); }
	| name EQUALSSIGN OPENPAREN expr.seq CLOSEPAREN 
		{ $$ = cxt.create(DeclareArgumentList{}, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $1, $4)); }
	| argument_declarations.pipelist comma.opt name literal
		{ $$ = cxt.graph.append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $3, $4)); }
	| argument_declarations.pipelist comma.opt name 
		{ $$ = cxt.graph.append( $1, @$, $3); }
	| argument_declarations.pipelist comma.opt name EQUALSSIGN literal
		{ $$ = cxt.graph.append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{true}, @$, $3, $5)); }
	| argument_declarations.pipelist comma.opt name OPENPAREN expr.seq CLOSEPAREN
		{ $$ = cxt.graph.append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $3, $5)); }
	| argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr.seq CLOSEPAREN
		{ $$ = cxt.graph.append( $1, @$, cxt.create(DeclareArgumentWithDefaultNode{false}, @$, $3, $6)); }
	;

argument_declarations
	: ARG SEMICOLON { $$ = cxt.create(DeclareArgumentList{}, @$); }
	| ARG argument_declarations.list comma.opt SEMICOLON { $$ = $2; }
	| ARG argument_declarations.list ELLIPSIS name SEMICOLON  
		{ $$ = cxt.graph.append( $2, @$, cxt.create(DeclareArgumentVariadicNode{}, @4, $4)); }
	| ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON 
		{ $$ = cxt.graph.append( $2, @$, cxt.create(DeclareArgumentVariadicNode{}, @4, $4), cxt.create(DeclareArgumentVariadicNode{}, @6, $6)); }
	| ARG ELLIPSIS name[args] SEMICOLON 
		{ 
			auto arg_list = cxt.create(DeclareArgumentList{}, @$);
			$$ = cxt.graph.append(arg_list, @$, cxt.create(DeclareArgumentVariadicNode{}, @args, $args)); 
		}
	| ARG ELLIPSIS name[args] COMMA name[vars] SEMICOLON 
		{
			auto arg_list = cxt.create(DeclareArgumentList{}, @$);
			$$ = cxt.graph.append(
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
		{ $$ = cxt.graph.append($2, @2, cxt.create(DeclareArgumentVariadicNode{}, @4, $4)); }
	| PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE 
		{ $$ = cxt.graph.append($2, @$, cxt.create(DeclareArgumentVariadicNode{}, @4, $4), cxt.create(DeclareArgumentVariadicNode{}, @6, $6)); }
	| PIPE ELLIPSIS name[args] PIPE 
		{ 
			auto arg_list = cxt.create(DeclareArgumentList{}, @$);
			$$ = cxt.graph.append(arg_list, @$, cxt.create(DeclareArgumentVariadicNode{}, @args, $args)); 
		}
	| PIPE ELLIPSIS name[args] COMMA name[vars] PIPE 
		{
			auto arg_list = cxt.create(DeclareArgumentList{}, @$);
			$$ = cxt.graph.append(
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
	: KEYBINOP expr.seq { $$ = cxt.create(KwArgNode{}, @$, cxt.create(SymbolNode{SymbolNode::Kind::KeyBinOp}, @1), $2); }
	| MULTIPLY expr.seq { $$ = cxt.create(VariadicArgNode{}, @$, $2); }
	| expr.seq { $$ = $1; }
	;

arguments.no_trailing
	: arguments.entries {  $$ = cxt.create(ArgumentList{}, @$, $1); }
	| arguments.no_trailing COMMA arguments.entries { $$ = cxt.graph.append($1, @$, $3); }
	;

arguments : arguments.no_trailing comma.opt {$$ = $1; };

arguments.paren : OPENPAREN arguments CLOSEPAREN { $$ = $2; }

arguments.maybe_paren 
	: %empty { $$ = cxt.create(ArgumentList{}, @$); }
	| OPENPAREN CLOSEPAREN
		{ $$ = cxt.create(ArgumentList{}, @$); }
	| arguments.paren { $$ = $1; }
	;

////////////////////////////////////////////////////////////////////////////////
// variable declarations
////////////////////////////////////////////////////////////////////////////////

variable_declarations.list.item 
	: name 
		{ $$ = $1; }
	| name EQUALSSIGN expr
		{ $$ = cxt.create(DeclareVariableWithDefaultNode{}, @$, $1, $3); }
	| name OPENPAREN expr.seq CLOSEPAREN
		{ $$ = cxt.create(DeclareVariableWithDefaultNode{}, @$, $1, $3); }
	| HASH name.list[names] comma.opt EQUALSSIGN expr
		{ $$ = cxt.create(VariablePack{}, @$, $names, $expr); }
	| HASH name.list[names] comma.opt OPENPAREN expr.seq[expr] CLOSEPAREN
		{ $$ = cxt.create(VariablePack{}, @$, $names, $expr); }
	;

variable_declarations.list
	: variable_declarations.list.item
		{ $$ = cxt.create(DeclareVariableList{}, @$, $1); }
	| variable_declarations.list COMMA variable_declarations.list.item
		{$$ = cxt.graph.append($1, $3); }
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
	| expr.seq 
		{ $$ = cxt.create(ArrayNode{}, @$, $1); }
	| expr.seq COLON expr.seq
		{ $$ = cxt.create(ArrayNode{}, @$, $1, $3); }
	| KEYBINOP expr.seq
		{ $$ = cxt.create(ArrayNode{}, @$, cxt.create(SymbolNode{SymbolNode::Kind::KeyBinOp}, @1), $2); }
	| literal.array.contents COMMA expr.seq
		{ $$ = cxt.graph.append($1, @$, $3); }
	| literal.array.contents COMMA expr.seq COLON expr.seq
		{ $$ = cxt.graph.append($1, @$, $3, $5); }
	| literal.array.contents COMMA KEYBINOP expr.seq
		{ $$ = cxt.graph.append($1, @$, cxt.create(SymbolNode{SymbolNode::Kind::KeyBinOp}, @3), $4); }
	;

literal.dictionary.entry 
	// This is a breaking change!
	// Previously you could have ( 1;2;3;4;5;6; : 10). now the expr.seq is invalid, but you can have ((1;2;3;4): 10)
	// This is necessary because of the scd format as (1;2;3;4;5;6;7;) is a valid file, and the parser would need infinite look ahead to figure out which is which.
	: expr COLON expr.seq 
		{ $$ = cxt.create(DictionaryEntryNode{}, @$, $1, $3); }
	| KEYBINOP expr.seq 
		{ $$ = cxt.create(DictionaryEntryNode{}, @$, cxt.create(SymbolNode{SymbolNode::Kind::KeyBinOp}, @1), $2); }
	;

literal.dictionary.entries 
	: %empty 
		{ $$ = cxt.create(DictionaryNode{}, @$); }
	| literal.dictionary.entry 
		{ $$ = cxt.create(DictionaryNode{}, @$, $1); }
	| literal.dictionary.entries COMMA literal.dictionary.entry
		{ $$ = cxt.graph.append($1, @$,  $3); }
	;

literal.dictionary 
	: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN 
		{ cxt.graph.location(*$2) = @$; $$ = $2; }
	;

literal.array	
	: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE 
		{  cxt.graph.location(*$2) = @$; $$ = $2; }	
	| HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE 
		{  
			cxt.graph.payload($3).is_immutable = true;
			cxt.graph.location(*$3) = @$;
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

name : NAME { $$ = cxt.create(NamedIdentifier{}, @$); } ;

// Used for destructuring
name.list 
	: name
		{ $$ = cxt.create(NameList{}, @$, $name); }
	| TILDE name 
		{ $$ = cxt.create(NameList{}, @$, cxt.create(EnvIdentifierNode{}, @$, $name)); }
	| name.list[list] COMMA name
		{ $$ = cxt.graph.append($list, @$, $name); }
	| name.list[list] COMMA TILDE name
		{ $$ = cxt.graph.append($list, @$, cxt.create(EnvIdentifierNode{},@$, $name)); }
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
	: SYMBOL_QUOTE { $$ = cxt.create(SymbolNode{SymbolNode::Kind::Quote}, @$); }
	| SYMBOL_SLASH { $$ = cxt.create(SymbolNode{SymbolNode::Kind::Slash}, @$); }
	;

string 	
	: STRINGLINE { $$ = cxt.create(StringLineList{}, @$, cxt.create(StringLineNode{}, @$)); }
	| string STRINGLINE { $$ = cxt.graph.append($1, cxt.create(StringLineNode{}, @2)); }
	;

integer	
	: INTEGER { $$ = cxt.create(IntNode{}, @$); }
	| INTEGER_RADIX { $$ = cxt.create(IntNode{IntNode::Kind::Radix}, @$); }
	| HEXADECIMAL { $$ = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, @$); }
	| MINUS integer %prec UMINUS 
		{
			// Reaches into the previous integer and changes its sign.
			cxt.graph.payload($2).sign = IntNode::Sign::Negative;
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
		{ cxt.graph.payload($2).sign = FloatNode::Sign::Negative; $$ = $2; }
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
		{ cxt.graph.payload($2).sign = AccidentalNode::Sign::Negative; $$ = $2; }
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
