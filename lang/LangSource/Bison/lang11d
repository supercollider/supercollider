%token	NAME INTEGER SC_FLOAT ACCIDENTAL SYMBOL STRING ASCII PRIMITIVENAME CLASSNAME CURRYARG
%token  VAR ARG CLASSVAR SC_CONST
%token	NILOBJ TRUEOBJ FALSEOBJ
%token	PSEUDOVAR
%token  ELLIPSIS DOTDOT PIE BEGINCLOSEDFUNC
%token  BADTOKEN INTERPRET
%token  BEGINGENERATOR LEFTARROW WHILE
%left	':'
%right  '='
%left	BINOP KEYBINOP '-' '<' '>' '*' '+' '|' READWRITEVAR
%left	'.'
%right  '`'
%right  UMINUS
%start  root

%{

#include <stdlib.h>
#include <string.h>
#include "PyrLexer.h"
#include "PyrParseNode.h"
#include "SC_Constants.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "InitAlloc.h"
#include "PredefinedSymbols.h"
#include "SimpleStack.h"

void bcopy(void *src, void *dst, size_t size) ;
int yyparse();
extern bool compilingCmdLine;
extern LongStack generatorStack;


%}
%error-verbose
%%

root	: classes
			{ gRootParseNode = (PyrParseNode*)$1; gParserResult = 1; }
		| classextensions
			{ gRootParseNode = (PyrParseNode*)$1; gParserResult = 1; }
		| INTERPRET cmdlinecode
			{ gRootParseNode = (PyrParseNode*)$2; gParserResult = 2; }
		;

classes : { $$ = 0; }
		|	classes classdef
			{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$2); }
		;

classextensions : classextension
				| classextensions classextension
				{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$2); }
				;

classdef	: classname superclass '{' classvardecls methods '}'
				{ $$ = (intptr_t)newPyrClassNode((PyrSlotNode*)$1, (PyrSlotNode*)$2,
					(PyrVarListNode*)$4, (PyrMethodNode*)$5, 0);
				}
			| classname '[' optname ']' superclass '{' classvardecls methods '}'
				{ $$ = (intptr_t)newPyrClassNode((PyrSlotNode*)$1, (PyrSlotNode*)$5,
					(PyrVarListNode*)$7, (PyrMethodNode*)$8,
					(PyrSlotNode*)$3);
				}
			;

classextension : '+' classname '{' methods '}'
				{
					$$ = (intptr_t)newPyrClassExtNode((PyrSlotNode*)$2, (PyrMethodNode*)$4);
				}
			;

optname		: { $$ = 0; }
			| name
			;

superclass	: { $$ = 0; }
			| ':' classname
				{ $$ = $2; }
			;

classvardecls	: { $$ = 0; }
				| classvardecls classvardecl
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$2); }
				;

classvardecl	: CLASSVAR rwslotdeflist ';'
					{ $$ = (intptr_t)newPyrVarListNode((PyrVarDefNode*)$2, varClass); }
				| VAR rwslotdeflist ';'
					{ $$ = (intptr_t)newPyrVarListNode((PyrVarDefNode*)$2, varInst); }
				| SC_CONST constdeflist ';'
					{ $$ = (intptr_t)newPyrVarListNode((PyrVarDefNode*)$2, varConst); }
				;

methods		: { $$ = 0; }
			| methods methoddef
				{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$2); }
			;

methoddef	: name '{' argdecls funcvardecls primitive methbody '}'
				{ $$ = (intptr_t)newPyrMethodNode((PyrSlotNode*)$1, (PyrSlotNode*)$5,
					(PyrArgListNode*)$3, (PyrVarListNode*)$4, (PyrParseNode*)$6, 0); }
			| '*' name '{' argdecls funcvardecls primitive methbody '}'
				{ $$ = (intptr_t)newPyrMethodNode((PyrSlotNode*)$2, (PyrSlotNode*)$6,
					(PyrArgListNode*)$4, (PyrVarListNode*)$5, (PyrParseNode*)$7, 1); }
			| binop '{' argdecls funcvardecls primitive methbody '}'
				{ $$ = (intptr_t)newPyrMethodNode((PyrSlotNode*)$1, (PyrSlotNode*)$5,
					(PyrArgListNode*)$3, (PyrVarListNode*)$4, (PyrParseNode*)$6, 0); }
			| '*' binop '{' argdecls funcvardecls primitive methbody '}'
				{ $$ = (intptr_t)newPyrMethodNode((PyrSlotNode*)$2, (PyrSlotNode*)$6,
					(PyrArgListNode*)$4, (PyrVarListNode*)$5, (PyrParseNode*)$7, 1); }
			;

optsemi		:
			| ';'
			;

optcomma	:
			| ','
			;

optequal	:
			| '='
			;

funcbody	: funretval
			| exprseq funretval
				{ $$ = (intptr_t)newPyrDropNode((PyrParseNode*)$1, (PyrParseNode*)$2); }
			;

cmdlinecode	: '(' funcvardecls1 funcbody ')'
				{ $$ = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)$2, (PyrParseNode*)$3, false); }
			| funcvardecls1 funcbody
				{ $$ = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)$1, (PyrParseNode*)$2, false); }
			| funcbody
				{ $$ = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)$1, false); }
			;

methbody	: retval
			| exprseq retval
				{ $$ = (intptr_t)newPyrDropNode((PyrParseNode*)$1, (PyrParseNode*)$2); }
			;

primitive	: { $$ = 0; }
			| primname optsemi
				{ $$ = $1; }
			;

retval	:
			{ $$ = (intptr_t)newPyrReturnNode(NULL); }
		| '^' expr optsemi
			{ $$ = (intptr_t)newPyrReturnNode((PyrParseNode*)$2); }
		;

funretval	:
			{ $$ = (intptr_t)newPyrBlockReturnNode(); }
		| '^' expr optsemi
			{ $$ = (intptr_t)newPyrReturnNode((PyrParseNode*)$2); }
		;

blocklist1	: blocklistitem
		| blocklist1 blocklistitem
				{
					$$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$2);
				}
			;

blocklistitem : blockliteral
				| generator
				;

blocklist	:	{ $$ = 0; }
			| blocklist1
			;

msgsend : name blocklist1
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$1, (PyrParseNode*)$2, 0, 0);
			}
		| '(' binop2 ')' blocklist1
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$2, (PyrParseNode*)$4, 0, 0);
			}
		| name '(' ')' blocklist1
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$1, NULL, NULL, (PyrParseNode*)$4);
			}
		| name '(' arglist1 optkeyarglist ')' blocklist
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$1, (PyrParseNode*)$3,
						(PyrParseNode*)$4, (PyrParseNode*)$6);
			}
		| '(' binop2 ')' '(' ')' blocklist1
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$2, NULL, NULL, (PyrParseNode*)$6);
			}
		| '(' binop2 ')' '(' arglist1 optkeyarglist ')' blocklist
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$2, (PyrParseNode*)$5,
						(PyrParseNode*)$6, (PyrParseNode*)$8);
			}
		| name '(' arglistv1 optkeyarglist ')'
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)$3)) {
					SetRaw(&((PyrPushNameNode*)$3)->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)$3,
					newPyrPushLitNode((PyrSlotNode*)$1, NULL));
				$$ = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)$4, 0);
			}
		| '(' binop2 ')' '(' arglistv1 optkeyarglist ')'
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_performList);
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)$5,
					newPyrPushLitNode((PyrSlotNode*)$2, NULL));
				$$ = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)$6, 0);
			}
		| classname '[' arrayelems ']'
			{ $$ = (intptr_t)newPyrDynListNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
		| classname blocklist1
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)$1);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, (PyrParseNode*)$2);
			}
		| classname '(' ')' blocklist
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)$1);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, NULL, (PyrParseNode*)$4);
			}
		| classname '(' keyarglist1 optcomma ')' blocklist
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)$1);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)$3, (PyrParseNode*)$6);
			}
		| classname '(' arglist1 optkeyarglist ')' blocklist
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)$1),
					(PyrParseNode*)$3);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)$4, (PyrParseNode*)$6);
			}
		| classname '(' arglistv1 optkeyarglist ')'
			{
				PyrSlotNode *selectornode, *selectornode2;
				PyrSlot slot, slot2;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)$1)) {
					SetRaw(&((PyrPushNameNode*)$1)->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				SetSymbol(&slot2, s_new);
				selectornode = newPyrSlotNode(&slot);
				selectornode2 = newPyrSlotNode(&slot2);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)$1),
					newPyrPushLitNode(selectornode2, NULL));
				args = linkNextNode(args, (PyrParseNode*)$3);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)$5, 0);
			}
		| expr '.' '(' ')' blocklist
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				$$ = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)$1, NULL, (PyrParseNode*)$5);
			}
		| expr '.' '(' keyarglist1 optcomma ')' blocklist
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				$$ = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)$1, (PyrParseNode*)$4, (PyrParseNode*)$7);
			}
		| expr '.' name '(' keyarglist1 optcomma ')' blocklist
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$3, (PyrParseNode*)$1,
					(PyrParseNode*)$5, (PyrParseNode*)$8);
			}
		| expr '.' '(' arglist1 optkeyarglist ')' blocklist
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					(PyrParseNode*)$4);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)$5, (PyrParseNode*)$7);
			}
		| expr '.' '(' arglistv1 optkeyarglist ')'
			{
				PyrSlotNode *selectornode;
				PyrSlot slot, slot2;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)$1)) {
					SetRaw(&((PyrPushNameNode*)$1)->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				SetSymbol(&slot2, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					newPyrPushLitNode(newPyrSlotNode(&slot2), NULL));
				args = linkNextNode(args, (PyrParseNode*)$4);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)$5, 0);
			}
		| expr '.' name '(' ')' blocklist
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$3, (PyrParseNode*)$1, NULL, (PyrParseNode*)$6);
			}
		| expr '.' name '(' arglist1 optkeyarglist ')' blocklist
			{
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$5);
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$3, args, (PyrParseNode*)$6, (PyrParseNode*)$8);
			}
		| expr '.' name '(' arglistv1 optkeyarglist ')'
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)$1)) {
					SetRaw(&((PyrPushNameNode*)$1)->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode(&slot);

				args = linkNextNode((PyrParseNode*)$1, newPyrPushLitNode((PyrSlotNode*)$3, NULL));
				args = linkNextNode(args, (PyrParseNode*)$5);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)$6, 0);
			}
		| expr '.' name blocklist
			{
				$$ = (intptr_t)newPyrCallNode((PyrSlotNode*)$3, (PyrParseNode*)$1, 0, (PyrParseNode*)$4);
			}
		;

generator : '{' ':' exprseq { pushls(&generatorStack, $3); pushls(&generatorStack, 1); } ',' qual '}'
			{
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)$6, false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				$$ = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0);
			}
		| '{' ';' exprseq { pushls(&generatorStack, $3); pushls(&generatorStack, 2); } ',' qual '}'
			{
				$$ = $6;
			}
		;

nextqual	:
				{
					// innermost part
					int action = popls(&generatorStack);
					PyrParseNode* expr = (PyrParseNode*)popls(&generatorStack);

					switch (action)
					{
						case 1 :
						{
							PyrSlot slot;
							SetSymbol(&slot, getsym("yield"));
							PyrSlotNode* selectornode = newPyrSlotNode(&slot);

							$$ = (intptr_t)newPyrCallNode(selectornode, expr, 0, 0);
						} break;
						case 2 :
						{
							$$ = (intptr_t)expr;
						} break;
					}
				}
			| ',' qual
				{ $$ = $2; }
			;

qual	: name LEFTARROW exprseq nextqual
			{
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)$3;
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)$1, NULL, 0);
						PyrArgListNode* args = newPyrArgListNode(var, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)$4, false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						$$ = (intptr_t)callnode;

					} else goto notoptimized1;
				} else {
					notoptimized1:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode(&slot);

					PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)$1, NULL, 0);
					PyrArgListNode* args = newPyrArgListNode(var, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)$4, false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					$$ = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
				}
			}
		| name name LEFTARROW exprseq nextqual
			{
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)$4;
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)$1, NULL, 0);
						PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)$2, NULL, 0);
						PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
						PyrArgListNode* args = newPyrArgListNode(vars, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)$5, false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						$$ = (intptr_t)callnode;

					} else goto notoptimized2;
				} else {
					notoptimized2:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode(&slot);

					PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)$1, NULL, 0);
					PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)$2, NULL, 0);
					PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
					PyrArgListNode* args = newPyrArgListNode(vars, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)$5, false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					$$ = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
				}
			}
		| VAR name '=' exprseq nextqual
			{
				PyrSlot slot;
				SetSymbol(&slot, s_value);
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)$2, NULL, 0);
				PyrArgListNode* args = newPyrArgListNode(var, NULL);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)$5, false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode(blocklit, (PyrParseNode*)$4);

				$$ = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
		| exprseq nextqual
			{
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)$2, false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)$1, blocklit);

				$$ = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
		| ':' ':' exprseq nextqual
			{
				$$ = (intptr_t)newPyrDropNode((PyrParseNode*)$3, (PyrParseNode*)$4);
			}
		| ':' WHILE exprseq nextqual
			{
				PyrSlot slot;
				SetSymbol(&slot, getsym("alwaysYield"));
				PyrSlotNode* selectornode1 = newPyrSlotNode(&slot);

				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode2 = newPyrSlotNode(&slot);

				SetNil(&slot);
				PyrParseNode *pushnil = (PyrParseNode*)newPyrPushLitNode(newPyrSlotNode(&slot), NULL);

				PyrParseNode *yieldNil = (PyrParseNode*)newPyrCallNode(selectornode1, pushnil, 0, 0);

				PyrParseNode *block1 = (PyrParseNode*)newPyrBlockNode(0, 0, yieldNil, false);
				PyrParseNode *blocklit1 = (PyrParseNode*)newPyrPushLitNode(NULL, block1);
				PyrParseNode *block2 = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)$4, false);
				PyrParseNode *blocklit2 = (PyrParseNode*)newPyrPushLitNode(NULL, block2);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)$3, blocklit2);
				PyrParseNode* args3 = (PyrParseNode*)linkNextNode(args2, blocklit1);

				$$ = (intptr_t)newPyrCallNode(selectornode2, args3, 0, 0);
			}
		;

expr1	: pushliteral
		| blockliteral
		| generator
		| pushname
		| curryarg
		| msgsend
		| '(' exprseq ')'
			{
				PyrParseNode* node = (PyrParseNode*)$2;
				node->mParens = 1;
				$$ = $2;
			}
		| '~' name
			{
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)$2, NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				$$ = (intptr_t)newPyrCallNode(selectornode, argnode, 0, 0);
			}
		|  '[' arrayelems ']'
			{ $$ = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)$2); }
		|	'(' valrange2 ')'
			{ $$ = $2; }
		|	'(' ':' valrange3 ')'
			{ $$ = $3; }
		|	'(' dictslotlist ')'
			{ $$ = (intptr_t)newPyrDynDictNode((PyrParseNode*)$2); }
		| pseudovar
			{ $$ = (intptr_t)newPyrPushNameNode((PyrSlotNode*)$1); }
		| expr1 '[' arglist1 ']'
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					(PyrParseNode*)$3);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| valrangex1
		;

valrangex1	: expr1 '[' arglist1 DOTDOT ']'
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)$3);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)$3);
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr1 '[' DOTDOT exprseq ']'
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)$4);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr1 '[' arglist1 DOTDOT exprseq ']'
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)$3);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)$3);
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)$5);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		;

valrangeassign : expr1 '[' arglist1 DOTDOT ']' '=' expr
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)$3);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)$3);
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)$7);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr1 '[' DOTDOT exprseq ']' '=' expr
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)$4);
				args = linkNextNode(args, (PyrParseNode*)$7);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr1 '[' arglist1 DOTDOT exprseq ']' '=' expr
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)$3);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)$3);
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)$5);
				args = linkNextNode(args, (PyrParseNode*)$8);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
	;

valrangexd	: expr '.' '[' arglist1 DOTDOT ']'
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)$4);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)$3);
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$4);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr '.' '[' DOTDOT exprseq ']'
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)$5);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr '.' '[' arglist1 DOTDOT exprseq ']'
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)$4);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)$3);
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$4);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)$6);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr '.' '[' arglist1 DOTDOT ']' '=' expr
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)$4);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)$3);
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$4);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)$8);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr '.' '[' DOTDOT exprseq ']' '=' expr
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)$5);
				args = linkNextNode(args, (PyrParseNode*)$8);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr '.' '[' arglist1 DOTDOT exprseq ']' '=' expr
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)$4);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)$3);
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$4);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)$6);
				args = linkNextNode(args, (PyrParseNode*)$9);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
	;

valrange2	: exprseq DOTDOT
			{
				// if this is not used in a 'do' or list comprehension, then should return an error.
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, nilnode);
				args = linkNextNode(args, nilnode2);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}

		| DOTDOT exprseq
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *zeronode;
				PyrSlot selectorSlot, nilSlot, zeroSlot;
				PyrParseNode* args;

				SetInt(&zeroSlot, 0);
				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				zeronode = newPyrPushLitNode(newPyrSlotNode(&zeroSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(zeronode, nilnode);
				args = linkNextNode(args, (PyrParseNode*)$2);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}

		| exprseq DOTDOT exprseq
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, nilnode);
				args = linkNextNode(args, (PyrParseNode*)$3);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}

		| exprseq ',' exprseq DOTDOT exprseq
			{
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					(PyrParseNode*)$3);
				args = linkNextNode(args, (PyrParseNode*)$5);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| exprseq ',' exprseq DOTDOT
			{
				// if this is not used in a 'do' or list comprehension, then should return an error.
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;
				PyrPushLitNode *nilnode;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					(PyrParseNode*)$3);
				args = linkNextNode(args, nilnode);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
	;

valrange3	: DOTDOT exprseq
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *zeronode;
				PyrSlot selectorSlot, nilSlot, zeroSlot;
				PyrParseNode* args;

				SetInt(&zeroSlot, 0);
				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				zeronode = newPyrPushLitNode(newPyrSlotNode(&zeroSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(zeronode, nilnode);
				args = linkNextNode(args, (PyrParseNode*)$2);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}

		| exprseq DOTDOT
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, nilnode);
				args = linkNextNode(args, nilnode2);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}

		| exprseq DOTDOT exprseq
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, nilnode);
				args = linkNextNode(args, (PyrParseNode*)$3);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}

		| exprseq ',' exprseq DOTDOT
			{
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3);
				args = linkNextNode(args, nilnode);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| exprseq ',' exprseq DOTDOT exprseq
			{
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					(PyrParseNode*)$3);
				args = linkNextNode(args, (PyrParseNode*)$5);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
	;

expr	: expr1
		| valrangexd
		| valrangeassign
		| classname { $$ = (intptr_t)newPyrPushNameNode((PyrSlotNode*)$1); }
		| expr '.' '[' arglist1 ']'
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					(PyrParseNode*)$4);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| '`' expr
			{
				PyrParseNode *node, *args;
				PyrSlotNode *slotnode;
				PyrSlot slot;

				SetSymbol(&slot, s_ref);
				slotnode = newPyrSlotNode(&slot);
				node = (PyrParseNode*)newPyrPushNameNode(slotnode);
				args = linkNextNode(node, (PyrParseNode*)$2);
				SetSymbol(&slot, s_new);
				slotnode = newPyrSlotNode(&slot);
				$$ = (intptr_t)newPyrCallNode(slotnode, args, 0, 0);
			}
		| expr binop2 adverb expr %prec BINOP
			{
				$$ = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)$2,
						(PyrParseNode*)$1, (PyrParseNode*)$4, (PyrParseNode*)$3);
			}
		| name '=' expr
			{
				$$ = (intptr_t)newPyrAssignNode((PyrSlotNode*)$1, (PyrParseNode*)$3, 0);
			}
		| '~' name '=' expr
			{
				PyrParseNode *argnode, *args;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)$2, NULL);
				args = linkNextNode(argnode, (PyrParseNode*)$4);
				SetSymbol(&slot, s_envirPut);
				selectornode = newPyrSlotNode(&slot);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr '.' name '=' expr
			{
				$$ = (intptr_t)newPyrSetterNode((PyrSlotNode*)$3,
						(PyrParseNode*)$1, (PyrParseNode*)$5);
			}
		| name '(' arglist1 optkeyarglist ')' '=' expr
			{
				if ($4 != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)$4);
					compileErrors++;
				}
				$$ = (intptr_t)newPyrSetterNode((PyrSlotNode*)$1,
						(PyrParseNode*)$3, (PyrParseNode*)$7);
			}
		| '#' mavars '=' expr
			{
				$$ = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)$2,
					(PyrParseNode*)$4, 0);
			}
		| expr1 '[' arglist1 ']' '=' expr
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					(PyrParseNode*)$3);
				args = linkNextNode( args, (PyrParseNode*)$6);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		| expr '.' '[' arglist1 ']' '=' expr
			{
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)$1,
					(PyrParseNode*)$4);
				args = linkNextNode( args, (PyrParseNode*)$7);
				$$ = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
		;

adverb  : { $$ = 0; }
		| '.' name { $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$2, NULL); }
		| '.' integer { $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$2, NULL); }
		| '.' '(' exprseq ')' { $$ = $3; }
		;

exprn	: expr
		| exprn ';' expr
			{
				$$ = (intptr_t)newPyrDropNode((PyrParseNode*)$1, (PyrParseNode*)$3);
			}
		;

exprseq : exprn optsemi
		;

arrayelems	: { $$ = 0; }
			| arrayelems1 optcomma
			  { $$ = $1; }
			;

arrayelems1	: exprseq
			| exprseq ':' exprseq
				{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			| keybinop exprseq
				{
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)$1, NULL);
					$$ = (intptr_t)linkNextNode(key, (PyrParseNode*)$2);
				}
			| arrayelems1 ',' exprseq
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			| arrayelems1 ',' keybinop exprseq
				{
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)$3, NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)$4);
					$$ = (intptr_t)linkNextNode((PyrParseNode*)$1, elems);
				}
			| arrayelems1 ',' exprseq ':' exprseq
				{
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)$3, (PyrParseNode*)$5);
					$$ = (intptr_t)linkNextNode((PyrParseNode*)$1, elems);
				}
			;

arglist1	: exprseq
			| arglist1 ',' exprseq
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			;

arglistv1	: '*' exprseq
				{ $$ = $2; }
			| arglist1 ',' '*' exprseq
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$4); }
			;

keyarglist1	: keyarg
			| keyarglist1 ',' keyarg
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			;

keyarg	: keybinop exprseq
				{ $$ = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)$1, (PyrParseNode*)$2); }
		;

optkeyarglist	: optcomma { $$ = 0; }
				| ',' keyarglist1 optcomma { $$ = $2; }
				;

mavars	: mavarlist
			{ $$ = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)$1, NULL); }
		| mavarlist ELLIPSIS name
			{ $$ = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)$1, (PyrSlotNode*)$3); }
		;

mavarlist	: name
			| mavarlist ',' name
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			;

slotliteral
		: integer	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
		| floatp	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
		| ascii		{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
		| string	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
		| symbol	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
		| trueobj	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
		| falseobj	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
		| nilobj	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
		| listlit	{ $$ = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)$1); }
		;

blockliteral : block	{ $$ = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)$1); }
			;

pushname	: name		{ $$ = (intptr_t)newPyrPushNameNode((PyrSlotNode*)$1); }
			;

pushliteral	: integer	{ $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$1, NULL); }
			| floatp	{ $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$1, NULL); }
			| ascii		{ $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$1, NULL); }
			| string	{ $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$1, NULL); }
			| symbol	{ $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$1, NULL); }
			| trueobj	{ $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$1, NULL); }
			| falseobj	{ $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$1, NULL); }
			| nilobj	{ $$ = (intptr_t)newPyrPushLitNode((PyrSlotNode*)$1, NULL); }
			| listlit	{ $$ = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)$1); }
			;

listliteral	: integer	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| floatp	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| ascii		{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| string	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| symbol	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| name		{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| trueobj	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| falseobj	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| nilobj	{ $$ = (intptr_t)newPyrLiteralNode((PyrSlotNode*)$1, NULL); }
			| listlit2	{ $$ = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)$1); }
			| dictlit2  { $$ = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)$1); }
			;

block	: '{' argdecls funcvardecls funcbody '}'
				{ $$ = (intptr_t)newPyrBlockNode((PyrArgListNode*)$2, (PyrVarListNode*)$3,
					(PyrParseNode*)$4, false); }
		| BEGINCLOSEDFUNC argdecls funcvardecls funcbody '}'
				{ $$ = (intptr_t)newPyrBlockNode((PyrArgListNode*)$2, (PyrVarListNode*)$3,
					(PyrParseNode*)$4, true); }
		;

funcvardecls	: { $$ = 0; }
				| funcvardecls funcvardecl
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$2); }
				;

funcvardecls1	: funcvardecl
				| funcvardecls1 funcvardecl
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$2); }
				;

funcvardecl	: VAR vardeflist ';'
				{ $$ = (intptr_t)newPyrVarListNode((PyrVarDefNode*)$2, varLocal); }
			;

argdecls	: { $$ = 0; }
			| ARG vardeflist ';'
				{
					$$ = (intptr_t)newPyrArgListNode((PyrVarDefNode*)$2, NULL);
				}
			| ARG vardeflist0 ELLIPSIS name ';'
				{
					$$ = (intptr_t)newPyrArgListNode((PyrVarDefNode*)$2, (PyrSlotNode*)$4);
				}
			| '|' slotdeflist '|'
				{
					$$ = (intptr_t)newPyrArgListNode((PyrVarDefNode*)$2, NULL);
				}
			| '|' slotdeflist0 ELLIPSIS name '|'
				{
					$$ = (intptr_t)newPyrArgListNode((PyrVarDefNode*)$2, (PyrSlotNode*)$4);
				}
			;

constdeflist	: constdef
				| constdeflist optcomma constdef
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
				;

constdef	: rspec name '=' slotliteral
				{ $$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$2, (PyrParseNode*)$4, $1); }
			;

slotdeflist0 	: { $$ = 0; }
				| slotdeflist
				;

slotdeflist	: slotdef
			| slotdeflist optcomma slotdef
				{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			;

slotdef		: name
				{ $$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$1, NULL, 0); }
			| name optequal slotliteral
				{ $$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$1, (PyrParseNode*)$3, 0); }
			| name optequal '(' exprseq ')'
				{
					PyrParseNode* node = (PyrParseNode*)$4;
					node->mParens = 1;
					$$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$1, node, 0);
				}
			;

vardeflist0 	: { $$ = 0; }
				| vardeflist
				;

vardeflist      : vardef
                        | vardeflist ',' vardef
                                { $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
                        ;

vardef          : name
                                { $$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$1, NULL, 0); }
                        | name '=' expr
                                { $$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$1, (PyrParseNode*)$3, 0); }
                        | name '(' exprseq ')'
                                {
									PyrParseNode* node = (PyrParseNode*)$3;
									node->mParens = 1;
									$$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$1, node, 0);
								}
                        ;

dictslotdef	: exprseq ':' exprseq
				{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			| keybinop exprseq
				{
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)$1, NULL);
					$$ = (intptr_t)linkNextNode(key, (PyrParseNode*)$2);
				}
			;

dictslotlist1	: dictslotdef
				| dictslotlist1 ',' dictslotdef
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
				;

dictslotlist	: { $$ = 0; }
				| dictslotlist1 optcomma
				;

rwslotdeflist	: rwslotdef
				| rwslotdeflist ',' rwslotdef
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
				;

rwslotdef		: rwspec name
					{ $$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$2, NULL, $1); }
				| rwspec name '=' slotliteral
					{ $$ = (intptr_t)newPyrVarDefNode((PyrSlotNode*)$2, (PyrParseNode*)$4, $1); }
				;

dictlit2	: '(' litdictslotlist ')'
				{ $$ = (intptr_t)newPyrLitDictNode((PyrParseNode*)$2); }
			;

litdictslotdef	: listliteral ':' listliteral
				{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			| keybinop listliteral
				{
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)$1, NULL);
					$$ = (intptr_t)linkNextNode(key, (PyrParseNode*)$2);
				}
			;

litdictslotlist1	: litdictslotdef
				| litdictslotlist1 ',' litdictslotdef
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
				;

litdictslotlist	: { $$ = 0; }
				| litdictslotlist1 optcomma
				;



listlit	: '#' '[' literallistc ']'
				{ $$ = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)$3); }
		| '#' classname  '[' literallistc ']'
				{ $$ = (intptr_t)newPyrLitListNode((PyrParseNode*)$2, (PyrParseNode*)$4); }
		;

listlit2	: '[' literallistc ']'
				{ $$ = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)$2); }
			| classname  '[' literallistc ']'
				{ $$ = (intptr_t)newPyrLitListNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
			;

literallistc	: { $$ = 0; }
				| literallist1 optcomma
				;

literallist1	: listliteral
				| literallist1 ',' listliteral
					{ $$ = (intptr_t)linkNextNode((PyrParseNode*)$1, (PyrParseNode*)$3); }
				;

rwspec	:  { $$ = rwPrivate; }
		| '<'
			{ $$ = rwReadOnly; }
		| READWRITEVAR
			{ $$ = rwReadWrite; }
		| '>'
			{ $$ = rwWriteOnly; }
		;

rspec	:  { $$ = rwPrivate; }
		| '<'
			{ $$ = rwReadOnly; }
		;

integer	: INTEGER { $$ = zzval; }
		| '-'INTEGER %prec UMINUS
			{
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				$$ = zzval;
			}
	;

floatr	: SC_FLOAT { $$ = zzval; }
		| '-' SC_FLOAT %prec UMINUS
			{
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				$$ = zzval;
			}
	;

accidental : ACCIDENTAL { $$ = zzval; }
			| '-' ACCIDENTAL %prec UMINUS
				{
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					$$ = zzval;
				}

pie		: PIE { $$ = zzval; }
	;

floatp	: floatr
		| accidental
		| floatr pie
			{
				PyrSlotNode *node;
				node = (PyrSlotNode*)$1;
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
		| integer pie
			{
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)$1;
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
		| pie
			{
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				$$ = zzval;
			}
		| '-' pie
			{
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				$$ = zzval;
			}
	;

name		: NAME { $$ = zzval; }
			| WHILE { $$ = zzval; }
		;

classname		: CLASSNAME { $$ = zzval; }
		;

primname		: PRIMITIVENAME { $$ = zzval; }
		;

trueobj		: TRUEOBJ { $$ = zzval; }
		;

falseobj	: FALSEOBJ { $$ = zzval; }
		;

nilobj		: NILOBJ { $$ = zzval; }
		;

ascii		: ASCII { $$ = zzval; }
		;

symbol		: SYMBOL { $$ = zzval; }
		;

string		: STRING { $$ = zzval; }
		;

pseudovar	: PSEUDOVAR { $$ = zzval; }
		;

binop	: BINOP { $$ = zzval; }
		| READWRITEVAR { $$ = zzval; }
		| '<'  { $$ = zzval; }
		| '>'  { $$ = zzval; }
		| '-'  { $$ = zzval; }
		| '*'  { $$ = zzval; }
		| '+'  { $$ = zzval; }
		| '|'  { $$ = zzval; }
	;

keybinop : KEYBINOP { $$ = zzval; }
		;

binop2  : binop
		| keybinop
		;

curryarg : CURRYARG { $$ = zzval; }

