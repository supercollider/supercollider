;; copyright 2003 stefan kersten <steve@k-hornz.de>
;;
;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License as
;; published by the Free Software Foundation; either version 2 of the
;; License, or (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful, but
;; WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;; General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
;; USA

(defvar sclang-running-xemacs (string-match "XEmacs\\|Lucid" emacs-version)
  "Set if running on XEmacs.")

(defun sclang-fill-syntax-table (table)
  ;; string
  (modify-syntax-entry ?\" "\"" table)
  (modify-syntax-entry ?\' "\"" table) ; no string systax class for single quotes
  ;; expression prefix
  (modify-syntax-entry ?~ "'" table)
  ;; escape
  (modify-syntax-entry ?\\ "\\" table)
  ;; character quote
  (modify-syntax-entry ?$  "/" table)
  ;; symbol
  (modify-syntax-entry ?_  "_" table)
  ;; symbol/punctuation
  (modify-syntax-entry ?!  "." table)
  (modify-syntax-entry ?%  "." table)
  (modify-syntax-entry ?&  "." table)
  (modify-syntax-entry ?*  ". 23n" table)
  (modify-syntax-entry ?+  "." table)
  (modify-syntax-entry ?-  "." table)
  (modify-syntax-entry ?/  ". 124b" table)
  (modify-syntax-entry ?<  "." table)
  (modify-syntax-entry ?=  "." table)
  (modify-syntax-entry ?>  "." table)
  (modify-syntax-entry ??  "." table)
  (modify-syntax-entry ?@  "." table)
  (modify-syntax-entry ?|  "." table)
  ;; punctuation
  (modify-syntax-entry ?:  "." table)
  (modify-syntax-entry ?\; "." table)
  (modify-syntax-entry ?\^ "." table)
  ;; parenthesis
  (modify-syntax-entry ?\( "()" table)
  (modify-syntax-entry ?\) ")(" table)
  (modify-syntax-entry ?\[ "(]" table)
  (modify-syntax-entry ?\] ")[" table)
  (modify-syntax-entry ?\{ "(}" table)
  (modify-syntax-entry ?\} "){" table)
  ;; comment end
  (modify-syntax-entry ?\n "> b" table)
  ;; Give CR the same syntax as newline, for selective-display
  (modify-syntax-entry ?\^m "> b" table)
  ;; return table
  table)

(defun sclang-mode-make-menu (title)
  (easy-menu-create-menu
   title
   '(
     ["Start Interpreter"	sclang-start :included (not (sclang-library-initialized-p))]
     ["Restart Interpreter"	sclang-start :included (sclang-library-initialized-p)]
     ["Stop Interpreter"	sclang-stop  :included (sclang-get-process)]
     ["Kill Interpreter"	sclang-kill  :included (sclang-get-process)]
     "-"
     ["Show Post Buffer"	sclang-show-post-buffer]
     ["Clear Post Buffer"	sclang-clear-post-buffer]
     "-"
     ["Switch To Workspace"	sclang-switch-to-workspace]
     "-"
     ["Evaluate Region"		sclang-eval-region]
     ["Evaluate Line"		sclang-eval-region-or-line]
     ["Evaluate Defun"		sclang-eval-defun]
     ["Evaluate Expression ..."	sclang-eval-expression]
     "-"
     ["Find Definitions ..."	sclang-find-definitions]
     ["Find References ..."	sclang-find-references]
     ["Pop Mark"		sclang-pop-definition-mark]
     ["Dump Interface"		sclang-dump-interface]
     "-"
     ["Index Help Topics"	sclang-index-help-topics]
     ["Find Help ..."		sclang-find-help]
     "-"
     ["Run Main"		sclang-main-run]
     ["Stop Main"		sclang-main-stop]
     )))

(defun sclang-fill-mode-map (map)
  ;; process control
  (define-key map "\C-c\C-l"	'sclang-start)
  ;; post buffer control
  (define-key map "\C-c<"	'sclang-clear-post-buffer)
  (define-key map "\C-c>"	'sclang-show-post-buffer)
  ;; workspace access
  (define-key map "\C-c\C-w"	'sclang-switch-to-workspace)
  ;; code evaluation
  (define-key map "\C-c\C-c"	'sclang-eval-region-or-line)
  (define-key map "\C-c\C-x"	'sclang-eval-region)
  (define-key map "\C-\M-x"	'sclang-eval-defun)
  (define-key map "\C-c\C-e"	'sclang-eval-expression)
  ;; language information
  (define-key map "\M-\t"	'sclang-complete-symbol)
  (define-key map "\C-c:"	'sclang-find-definitions)
  (define-key map "\C-c;"	'sclang-find-references)
  (define-key map "\C-c}"	'sclang-pop-definition-mark)
  (define-key map "\C-c{"	'sclang-dump-interface)
  ;; documentation access
  (define-key map "\C-c\C-h"	'sclang-find-help)
  ;; language control
  (define-key map "\C-c\C-r"	'sclang-main-run)
  (define-key map "\C-c\C-s"	'sclang-main-stop)
  ;; menu
  (let ((title "SCLang"))
    (define-key map [menu-bar sclang] (cons title (sclang-mode-make-menu title))))
  ;; return map
  map)

;; =====================================================================
;; font-lock support
;; =====================================================================

(defvar sclang-font-lock-keyword-list 
  '(
    "arg"
    "classvar"
    "var"
    "super"
    "this"
    "thisContext"
    "thisMethod"
    "thisProcess"
    )
  "*List of keywords to highlight in SCLang mode.")

(defvar sclang-font-lock-builtin-list
  '(
    "nil"
    "true"
    "false"
    "inf"
    "pi"
    )
  "*List of builtins to highlight in SCLang mode.")

(defvar sclang-font-lock-method-list
  '(
    "if" "while" "loop" "forBy"
    "ar" "kr"
    )
  "*List of methods to highlight in SCLang mode.")

(defvar sclang-font-lock-class-keywords nil
  "Regular expression matching class names in font-lock-mode.")

(defvar sclang-font-lock-keywords-1 nil
  "Subdued level highlighting for SCLang mode.")

(defvar sclang-font-lock-keywords-2 nil
  "Medium level highlighting for SCLang mode.")

(defvar sclang-font-lock-keywords-3 nil
  "Gaudy level highlighting for SCLang mode.")

(defconst sclang-font-lock-keywords nil
  "Default expressions to highlight in SCLang mode.")

(defconst sclang-font-lock-defaults '((sclang-font-lock-keywords
				       sclang-font-lock-keywords-1
				       sclang-font-lock-keywords-2
				       sclang-font-lock-keywords-3
				       )
				      nil nil
				      nil
				      beginning-of-defun
				      ))

(defun sclang-set-font-lock-class-keywords ()
  (setq sclang-font-lock-class-keywords
	(concat
	 "\\<"
	 (or (and sclang-symbol-table
		  (condition-case nil
		      (concat
		       "\\(Meta_\\)?"
		       (regexp-opt
			(remove-if-not (lambda (name)
					 (and (sclang-class-name-p name)
					      (not (match-string 1 name))))
				       sclang-symbol-table)
			t))
		    (error nil)))
	     sclang-class-name-regexp)
	 "\\>")))

(defun sclang-font-lock-syntactic-face (state)
  (cond ((eq (nth 3 state) ?')
	 ;; symbol
	 'font-lock-constant-face)
	((nth 3 state)
	 ;; string
	 'font-lock-string-face)
	((nth 4 state)
	 ;; comment
	 'font-lock-comment-face)))

(defun sclang-set-font-lock-keywords ()
  (unless sclang-font-lock-class-keywords
    (sclang-set-font-lock-class-keywords))

  (setq
   ;; level 1
   sclang-font-lock-keywords-1
   (list
    ;; keywords
    (cons (regexp-opt (sort (copy-list sclang-font-lock-keyword-list) 'string<) 'words)
	  'font-lock-keyword-face)
    ;; builtins
    (cons (regexp-opt (sort (copy-list sclang-font-lock-builtin-list) 'string<) 'words)
	  'font-lock-builtin-face)
    ;; constants
    (cons "\\s/\\s\\?." 'font-lock-constant-face) ; characters
    (cons (concat "\\\\\\(" sclang-symbol-regexp "\\)")
	  'font-lock-constant-face)	; symbols
    )
   ;; level 2
   sclang-font-lock-keywords-2
   (append
    sclang-font-lock-keywords-1
    (list
     ;; variables
     (cons (concat "\\s'\\(" sclang-identifier-regexp "\\)")
	   'font-lock-variable-name-face) ; environment variables
     (cons (concat "\\<\\(" sclang-identifier-regexp "\\)\\>:")	; keyword arguments
	   'font-lock-variable-name-face)
     ;; method definitions
     (cons sclang-method-definition-regexp
	   (list 1 'font-lock-function-name-face))
     ;; methods
     (cons (regexp-opt (sort (copy-list sclang-font-lock-method-list) 'string<) 'words)
	   'font-lock-function-name-face)
     ))
   ;; level 3
   sclang-font-lock-keywords-3
   (append
    sclang-font-lock-keywords-2
    (list
     ;; classes
     (cons sclang-font-lock-class-keywords
	   'font-lock-type-face)
     ))
   ;; default level
   sclang-font-lock-keywords sclang-font-lock-keywords-1
   ))

(defun sclang-update-font-lock ()
  "Update font-lock information in all buffers displaying SCLang code."
  (sclang-set-font-lock-class-keywords)
  (dolist (buffer (buffer-list))
    (with-current-buffer buffer
      (when (eq major-mode 'sclang-mode)
	(sclang-set-font-lock-keywords)
	(when font-lock-mode
	  (font-lock-mode)
	  (font-lock-mode))))))

;; =====================================================================
;; indentation
;; =====================================================================

(defcustom sclang-indent-level 4
  "*Indentation offset for SCLang statements."
  :group 'sclang-mode
  :type 'integer)

(defun sclang-indent-line ()
  "Indent current line as sclang code.
Return the amount the indentation changed by."
  (let ((indent (calculate-sclang-indent))
	beg shift-amt
	(case-fold-search nil)
	(pos (- (point-max) (point))))
    (beginning-of-line)
    (setq beg (point))
    ;;     (cond ((eq indent nil)
    ;; 	   (setq indent (current-indentation)))
    ;; 	  (t
    ;; 	   (skip-chars-forward " \t")
    ;; 	   (if (listp indent) (setq indent (car indent)))
    ;; 	   (if (memq (following-char) '(?} ?\) ?\]))
    ;; 	       (setq indent (- indent sclang-indent-level)))))
    (skip-chars-forward " \t")
    (setq shift-amt (- indent (current-column)))
    (if (zerop shift-amt)
	(if (> (- (point-max) pos) (point))
	    (goto-char (- (point-max) pos)))
      (delete-region beg (point))
      (indent-to indent)
      ;; If initial point was within line's indentation,
      ;; position after the indentation.  Else stay at same point in text.
      (if (> (- (point-max) pos) (point))
	  (goto-char (- (point-max) pos))))
    shift-amt))

(defun calculate-sclang-indent (&optional parse-start)
  "Return appropriate indentation for current line as sclang code.
Returns the column to indent to."
  (save-excursion
    (beginning-of-line)
    (let ((indent-point (point))
	  (case-fold-search nil)
	  state)
      (if parse-start
	  (goto-char parse-start)
	(beginning-of-defun))
      (while (< (point) indent-point)
	(setq state (parse-partial-sexp (point) indent-point 0)))
      (let* ((containing-sexp (nth 1 state))
	     (inside-string-p (nth 3 state))
	     (inside-comment-p (nth 4 state)))
	(cond (inside-string-p
	       ;; inside string; don't change indentation
	       (current-indentation))
	      ((integerp inside-comment-p)
	       ;; inside comment
	       (let ((base 0)
		     (offset sclang-indent-level))
		 (if containing-sexp
		     (save-excursion
		       (goto-char containing-sexp)
		       (setq base (current-indentation))))
		 (if (and (= inside-comment-p 1)
			  (save-excursion
			    (back-to-indentation)
			    (looking-at "\\*/")))
		     (setq offset 0))
		 (+ base sclang-indent-level offset)))
	      ((null containing-sexp)
	       0)
	      (t
	       (back-to-indentation)
	       (let ((inc (if (looking-at "\\s)")
			      0 sclang-indent-level)))
		 (goto-char containing-sexp)
		 ;; add indent-level to indentation of containing sexp
		 (+ (current-indentation) inc))))))))

;; =====================================================================
;; sclang-mode
;; =====================================================================

(defun sclang-mode-set-local-variables ()
  (set (make-local-variable 'require-final-newline) nil)
  ;; indentation
  (set (make-local-variable 'indent-line-function)
       'sclang-indent-line)
  (set (make-local-variable 'tab-width) 4)
  (set (make-local-variable 'indent-tabs-mode) t)
  ;; comment formatting
  (set (make-local-variable 'comment-start) "// ")
  (set (make-local-variable 'comment-end) "")
  (set (make-local-variable 'comment-column) 40)
  (set (make-local-variable 'comment-start-skip) "/\\*+ *\\|//+ *")
  ;;        "\\(^\\|\\s-\\);?// *")
  (set (make-local-variable 'comment-multi-line) t)
  ;; parsing and movement
  (set (make-local-variable 'parse-sexp-ignore-comments) t)
  (set (make-local-variable 'beginning-of-defun-function)
       'sclang-beginning-of-defun)
  (set (make-local-variable 'end-of-defun-function)
       'sclang-end-of-defun)
  ;; paragraph formatting
  ;;   (set (make-local-variable 'paragraph-start) (concat "$\\|" page-delimiter))
  ;; mostly copied from c++-mode, seems to work
  (set (make-local-variable 'paragraph-start) "[ 	]*\\(//+\\|\\**\\)[ 	]*$\\|^")
  (set (make-local-variable 'paragraph-separate) paragraph-start)
  (set (make-local-variable 'paragraph-ignore-fill-prefix) t)
  (set (make-local-variable 'adaptive-fille-mode) t)
  (set (make-local-variable 'adaptive-fill-regexp) "[ 	]*\\(//+\\|\\**\\)[ 	]*\\([ 	]*\\([-|#;>*]+[ 	]*\\|(?[0-9]+[.)][ 	]*\\)*\\)")
  ;; font lock
  (set (make-local-variable 'font-lock-syntactic-face-function)
       'sclang-font-lock-syntactic-face)
  (set (make-local-variable 'font-lock-defaults)
       sclang-font-lock-defaults)
  ;; ---
  nil)

(defvar sclang-mode-map (sclang-fill-mode-map (make-sparse-keymap))
  "Keymap used in SuperCollider mode.")

(defvar sclang-mode-syntax-table (sclang-fill-syntax-table (make-syntax-table))
  "Syntax table used in SuperCollider mode.")

(defcustom sclang-mode-hook nil
  "*Hook run when entering SCLang mode."
  :group 'sclang-mode
  :type 'hook)

(defun sclang-mode ()
  "Major mode for editing SuperCollider language (SCLang) code.

\\{sclang-mode-map}
"
  (interactive)
  (kill-all-local-variables)
  (set-syntax-table sclang-mode-syntax-table)
  (use-local-map sclang-mode-map)
  (setq mode-name "SCLang")
  (setq major-mode 'sclang-mode)
  (sclang-mode-set-local-variables)
  (sclang-set-font-lock-keywords)
  (run-hooks 'sclang-mode-hook))

;; =====================================================================
;; module initialization
;; =====================================================================

(add-to-list 'auto-mode-alist '("\\.sc$" . sclang-mode))
(add-to-list 'interpreter-mode-alist '("sclang" . sclang-mode))

(provide 'sclang-mode)

;; EOF
