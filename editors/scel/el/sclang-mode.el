;; copyright 2003-2005 stefan kersten <steve@k-hornz.de>
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
;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
;; USA

(eval-when-compile
  (require 'cl)
  (load "cl-seq" nil t)
  (require 'font-lock)
  (require 'sclang-util))

(require 'sclang-interp)
(require 'sclang-language)
(require 'sclang-dev)

(defun sclang-fill-syntax-table (table)
  ;; string
  (modify-syntax-entry ?\" "\"" table)
  (modify-syntax-entry ?\' "\"" table) ; no string syntax class for single quotes
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
  (modify-syntax-entry ?^ "." table)
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
	 ["Start Interpreter"		sclang-start :included (not (sclang-library-initialized-p))]
	 ["Restart Interpreter"		sclang-start :included (sclang-library-initialized-p)]
	 ["Recompile Class Library"	sclang-recompile :included (sclang-library-initialized-p)]
	 ["Stop Interpreter"		sclang-stop	 :included (sclang-get-process)]
	 ["Kill Interpreter"		sclang-kill	 :included (sclang-get-process)]
	 "-"
	 ["Show Post Buffer"		sclang-show-post-buffer]
	 ["Clear Post Buffer"		sclang-clear-post-buffer]
	 "-"
	 ["Switch To Workspace"		sclang-switch-to-workspace]
	 "-"
	 ["Evaluate Region"			sclang-eval-region]
	 ["Evaluate Line"			sclang-eval-region-or-line]
	 ["Evaluate Defun"			sclang-eval-defun]
	 ["Evaluate Expression ..."	sclang-eval-expression]
	 ["Evaluate Document"		sclang-eval-document]
	 "-"
	 ["Find Definitions ..."	sclang-find-definitions]
	 ["Find References ..."		sclang-find-references]
	 ["Pop Mark"				sclang-pop-definition-mark]
	 ["Show Method Arguments"	sclang-show-method-args]
	 ["Complete keyword"		sclang-complete-symbol]
	 ["Dump Interface"			sclang-dump-interface]
	 ["Dump Full Interface"		sclang-dump-full-interface]
	 "-"
	 ["Index Help Topics"		sclang-index-help-topics]
	 ["Find Help ..."			sclang-find-help]
	 ["Switch to Help Browser"	sclang-goto-help-browser]
	 ["Open Help GUI"			sclang-open-help-gui]
	 "-"
	 ["Run Main"				sclang-main-run]
	 ["Stop Main"				sclang-main-stop]
	 ["Show Server Panels"		sclang-show-server-panel]
	 )))

(defun sclang-fill-mode-map (map)
  ;; process control
  (define-key map "\C-c\C-l"		'sclang-recompile)
  (define-key map "\C-c\C-o"		'sclang-start)
  ;; post buffer control
  (define-key map "\C-c<"			'sclang-clear-post-buffer)
  (define-key map "\C-c>"			'sclang-show-post-buffer)
  ;; workspace access
  (define-key map "\C-c\C-w"		'sclang-switch-to-workspace)
  ;; code evaluation
  (define-key map "\C-c\C-c"		'sclang-eval-region-or-line)
  (define-key map "\C-c\C-d"		'sclang-eval-region)
  (define-key map "\C-\M-x"			'sclang-eval-defun)
  (define-key map "\C-c\C-e"		'sclang-eval-expression)
  (define-key map "\C-c\C-f"		'sclang-eval-document)
  ;; language information
  (define-key map "\C-c\C-n"		'sclang-complete-symbol)
  (define-key map "\M-\t"			'sclang-complete-symbol)
  (define-key map "\C-c:"			'sclang-find-definitions)
  (define-key map "\C-c;"			'sclang-find-references)
  (define-key map "\C-c}"			'sclang-pop-definition-mark)
  (define-key map "\C-c\C-m"		'sclang-show-method-args)
  (define-key map "\C-c{"			'sclang-dump-full-interface)
  (define-key map "\C-c["			'sclang-dump-interface)
  ;; documentation access
  (define-key map "\C-c\C-h"		'sclang-find-help)
  (define-key map "\C-\M-h"			'sclang-goto-help-browser)
  (define-key map "\C-c\C-y"		'sclang-open-help-gui)
  (define-key map "\C-ch"			'sclang-find-help-in-gui)
  ;; language control
  (define-key map "\C-c\C-r"		'sclang-main-run)
  (define-key map "\C-c\C-s"		'sclang-main-stop)
  (define-key map "\C-c\C-p"		'sclang-show-server-panel)
  (define-key map "\C-c\C-k"		'sclang-edit-dev-source)
  ;; electric characters
  (define-key map "}"				'sclang-electric-brace)
  (define-key map ")"				'sclang-electric-brace)
  (define-key map "]"				'sclang-electric-brace)
  (define-key map "/"				'sclang-electric-slash)
  (define-key map "*"				'sclang-electric-star)
  ;; menu
  (let ((title "SCLang"))
	(define-key map [menu-bar sclang] (cons title (sclang-mode-make-menu title))))
  ;; return map
  map)

;; =====================================================================
;; font-lock support
;; =====================================================================

(defconst sclang-font-lock-keyword-list
  '(
    "arg"
    "classvar"
    "const"
    "super"
    "this"
    "thisFunction"
    "thisFunctionDef"
    "thisMethod"
    "thisProcess"
    "thisThread"
    "var"
    )
  "*List of keywords to highlight in SCLang mode.")

(defconst sclang-font-lock-builtin-list
  '(
    "false"
    "inf"
    "nil"
    "true"
    )
  "*List of builtins to highlight in SCLang mode.")

(defconst sclang-font-lock-method-list
  '(
    "ar"
    "for"
    "forBy"
    "if"
    "ir"
    "kr"
    "tr"
    "loop"
    "while"
    )
  "*List of methods to highlight in SCLang mode.")

(defconst sclang-font-lock-error-list
  '(
    "die"
    "error"
    "exit"
    "halt"
    "verboseHalt"
    "warn"
    )
  "*List of methods signalling errors or warnings.")

(defvar sclang-font-lock-class-keywords nil)

(defvar sclang-font-lock-keywords-1 nil
  "Subdued level highlighting for SCLang mode.")

(defvar sclang-font-lock-keywords-2 nil
  "Medium level highlighting for SCLang mode.")

(defvar sclang-font-lock-keywords-3 nil
  "Gaudy level highlighting for SCLang mode.")

(defvar sclang-font-lock-keywords nil
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

(defun sclang-font-lock-class-keyword-matcher (limit)
  (let ((regexp (or sclang-font-lock-class-keywords
		    (concat "\\<" sclang-class-name-regexp "\\>")))
	(case-fold-search nil))
    (re-search-forward regexp limit t)))

(defun sclang-set-font-lock-keywords ()
  (setq
   ;; level 1
   sclang-font-lock-keywords-1
   (list
    ;; keywords
    (cons (regexp-opt sclang-font-lock-keyword-list'words)
	  'font-lock-keyword-face)
    ;; builtins
    (cons (regexp-opt sclang-font-lock-builtin-list 'words)
	  'font-lock-builtin-face)
    ;; pi is a special case
    (cons "\\<\\([0-9]+\\(\\.\\)\\)pi\\>" 'font-lock-builtin-face)
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
     (cons (regexp-opt sclang-font-lock-method-list 'words)
	   'font-lock-function-name-face)
     ;; errors
     (cons (regexp-opt sclang-font-lock-error-list 'words)
	   'font-lock-warning-face)
     ))
   ;; level 3
   sclang-font-lock-keywords-3
   (append
    sclang-font-lock-keywords-2
    (list
     ;; classes
     (cons 'sclang-font-lock-class-keyword-matcher 'font-lock-type-face)
;;      (cons (concat "\\<" sclang-class-name-regexp "\\>") 'font-lock-type-face)
     ))
   ;; default level
   sclang-font-lock-keywords sclang-font-lock-keywords-1
   ))

(defun sclang-update-font-lock ()
  "Update font-lock information in all sclang-mode buffers."
  (setq sclang-font-lock-class-keywords
	(and sclang-symbol-table
	     (let* ((list (remove-if
			   (lambda (x) (or (not (sclang-class-name-p x))
					   (sclang-string-match "^Meta_" x)))
			   sclang-symbol-table))
		    ;; need to set this for large numbers of classes
		    (max-specpdl-size (* (length list) 2)))
	       (condition-case nil
		   (concat "\\<\\(?:Meta_\\)?\\(?:" (regexp-opt list) "\\)\\>")
		 (error nil)))))
  ;; too expensive
  ;;   (dolist (buffer (buffer-list))
  ;;     (with-current-buffer buffer
  ;;       (and (eq major-mode 'sclang-mode)
  ;; 	   (eq t (car font-lock-keywords))
  ;; 	   (setq font-lock-keywords (cdr font-lock-keywords)))))
  (if (eq major-mode 'sclang-mode)
      (font-lock-fontify-buffer)))

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
    (skip-chars-forward " \t")
    (setq shift-amt (- indent (current-column)))
    (if (zerop shift-amt)
	(if (> (- (point-max) pos) (point))
	    (goto-char (- (point-max) pos)))
      (delete-region beg (point))
      (indent-to indent)
      ;; if initial point was within line's indentation, position
      ;; after the indentation, else stay at same point in text.
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
	       ;; inside string: no change
	       (current-indentation))
	      ((integerp inside-comment-p)
	       ;; inside comment
	       (let ((base (if containing-sexp
			       (save-excursion
				 (goto-char containing-sexp)
				 (+ (current-indentation) sclang-indent-level))
			     0))
		     (offset (* sclang-indent-level
				(- inside-comment-p
				   (if (save-excursion
					 (back-to-indentation)
					 (looking-at "\\*/"))
				       1 0)))))
		 (+ base offset)))
	      ((null containing-sexp)
	       ;; top-level: no indentation
	       0)
	      (t
	       (back-to-indentation)
	       (let ((open-paren (and (looking-at "\\s)")
				      (matching-paren (char-after))))
		     (indent (current-indentation)))
		 (goto-char containing-sexp)
		 (if (or (not open-paren) (eq open-paren (char-after)))
		     (cond ((progn (beginning-of-line) (looking-at sclang-block-regexp)) 0)
			   (open-paren (current-indentation))
			   (t (+ (current-indentation) sclang-indent-level)))
		   ;; paren mismatch: do nothing
		   indent))))))))

;; =====================================================================
;; electric character commands
;; =====================================================================

(defun sclang-electric-brace (arg)
  (interactive "*P")
  (self-insert-command (prefix-numeric-value arg))
  (and (save-excursion
	 (beginning-of-line)
	 (looking-at "\\s *\\s)"))
       (indent-according-to-mode)))

(defun sclang-electric-slash (arg)
  (interactive "*P")
  (let* ((char (char-before))
	 (indent-p (or (eq char ?/)
		       (eq char ?*))))
    (self-insert-command (prefix-numeric-value arg))
    (if indent-p (indent-according-to-mode))))

(defun sclang-electric-star (arg)
  (interactive "*P")
  (let ((indent-p (eq (char-before) ?/)))
    (self-insert-command (prefix-numeric-value arg))
    (if indent-p (indent-according-to-mode))))

;; =====================================================================
;; document interface
;; =====================================================================

(defvar sclang-document-id nil)
(defvar sclang-document-state nil)
(defvar sclang-document-envir nil)

(defvar sclang-document-counter 0)
(defvar sclang-document-list nil)
(defvar sclang-current-document nil
  "Currently active document.")

(defvar sclang-document-idle-timer nil)

(defconst sclang-document-property-map
  '((sclang-document-name . (prSetTitle (buffer-name)))
    (sclang-document-path . (prSetFileName (buffer-file-name)))
    (sclang-document-listener-p . (prSetIsListener (eq (current-buffer) (sclang-get-post-buffer))))
    (sclang-document-editable-p . (prSetEditable (not buffer-read-only)))
    (sclang-document-edited-p . (prSetEdited (buffer-modified-p)))))

(defmacro sclang-next-document-id ()
  `(incf sclang-document-counter))

(defun sclang-document-list ()
  sclang-document-list)

(defun sclang-document-id (buffer)
  (cdr (assq 'sclang-document-id (buffer-local-variables buffer))))

(defun sclang-document-p (buffer)
  (integerp (sclang-document-id buffer)))

(defmacro with-sclang-document (buffer &rest body)
  `(when (sclang-document-p buffer)
     (with-current-buffer buffer
       ,@body)))

(defun sclang-get-document (id)
  (find-if (lambda (doc) (eq id (sclang-document-id doc)))
	   (sclang-document-list)))

(defun sclang-init-document ()
  (set (make-local-variable 'sclang-document-id) (sclang-next-document-id))
  (set (make-local-variable 'sclang-document-envir) nil)
  (dolist (assoc sclang-document-property-map)
    (set (make-local-variable (car assoc)) nil))
  (pushnew (current-buffer) sclang-document-list))

(defun sclang-document-update-property-1 (assoc &optional force)
  (when (consp assoc)
    (let* ((key (car assoc))
	   (prop (cdr assoc))
	   (prev-value (eval key))
	   (cur-value (eval (cadr prop))))
      (when (or force (not (equal prev-value cur-value)))
	(set key cur-value)
	(sclang-perform-command-no-result
	 'documentSetProperty sclang-document-id
	 (car prop) cur-value)))))

(defun sclang-document-update-property (key &optional force)
  (sclang-document-update-property-1 (assq key sclang-document-property-map) force))

(defun sclang-document-update-properties (&optional force)
  (dolist (assoc sclang-document-property-map)
    (sclang-document-update-property-1 assoc force)))

(defun sclang-make-document ()
  (sclang-perform-command-no-result 'documentNew sclang-document-id)
  (sclang-document-update-properties t))

(defun sclang-close-document (buffer)
  (with-sclang-document
   buffer
   (setq sclang-document-list (delq buffer sclang-document-list))
   (sclang-perform-command-no-result
    'documentClosed sclang-document-id)))

(defun sclang-set-current-document (buffer &optional force)
  (when (or force (not (eq buffer sclang-current-document)))
    (setq sclang-current-document buffer)
    (sclang-perform-command-no-result 'documentSetCurrent (sclang-document-id buffer))
    t))

(defun sclang-document-library-startup-hook-function ()
  (dolist (buffer (sclang-document-list))
    (with-current-buffer buffer
      (sclang-make-document)))
  (sclang-set-current-document (current-buffer) t))

(defun sclang-document-kill-buffer-hook-function ()
  (sclang-close-document (current-buffer)))

(defun sclang-document-post-command-hook-function ()
  (when (and (sclang-library-initialized-p)
	     (sclang-document-p (current-buffer)))
    (sclang-document-update-properties))
  (sclang-set-current-document (current-buffer)))

(defun sclang-document-change-major-mode-hook-function ()
  (sclang-close-document (current-buffer)))

;; =====================================================================
;; command handlers
;; =====================================================================

(sclang-set-command-handler
 '_documentOpen
 (lambda (arg)
   (multiple-value-bind (file-name region-start region-length) arg
     (let ((buffer (get-file-buffer file-name)))
       (unless buffer
	 (setf buffer (find-file-noselect file-name)))
       (when buffer
	 (unless (sclang-document-p buffer)
	   (with-current-buffer buffer (sclang-mode)))
	 (goto-char (max (point-min) (min (point-max) region-start)))
	 ;; TODO: how to activate region in transient-mark-mode?
	 (sclang-document-id buffer))))))

(sclang-set-command-handler
 '_documentNew
 (lambda (arg)
   (multiple-value-bind (name str make-listener) arg
     (let ((buffer (generate-new-buffer name)))
       (with-current-buffer buffer
	 (insert str)
	 (set-buffer-modified-p nil)
	 (sclang-mode))
       (sclang-document-id buffer)))))

(sclang-set-command-handler
 '_documentClose
 (lambda (arg)
   (let ((doc (and (integerp arg) (sclang-get-document arg))))
     (and doc (kill-buffer doc)))
   nil))

(sclang-set-command-handler
 '_documentRename
 (lambda (arg)
   (multiple-value-bind (id name) arg
     (when (stringp name)
       (let ((doc (and (integerp id) (sclang-get-document id))))
	 (when doc
	   (with-current-buffer doc
	     (rename-buffer name t)
	     (sclang-document-update-property 'sclang-document-name))))))
   nil))

(sclang-set-command-handler
 '_documentSetEditable
 (lambda (arg)
   (multiple-value-bind (id flag) arg
     (let ((doc (and (integerp id) (sclang-get-document id))))
       (when doc
	 (with-current-buffer doc
	   (setq buffer-read-only (not flag))
	   (sclang-document-update-property 'sclang-editable-p)))))
   nil))

(sclang-set-command-handler
 '_documentSwitchTo
 (lambda (arg)
   (let ((doc (and (integerp arg) (sclang-get-document arg))))
     (and doc (switch-to-buffer doc)))
   nil))

(sclang-set-command-handler
 '_documentPutString
(lambda (arg)
   (multiple-value-bind (id str) arg
     (let ((doc (and (integerp id) (sclang-get-document id))))
       (when doc
	 (with-current-buffer doc
	 (insert str)
	 )
       nil)))))

(sclang-set-command-handler
 '_documentPopTo
 (lambda (arg)
   (let ((doc (and (integerp arg) (sclang-get-document arg))))
     (and doc (display-buffer doc)))
   nil))

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
  (set (make-local-variable 'paragraph-start)
       "[ \t]*\\(//+\\|\\**\\)[ \t]*$\\|^")
  (set (make-local-variable 'paragraph-separate) paragraph-start)
  (set (make-local-variable 'paragraph-ignore-fill-prefix) t)
  (set (make-local-variable 'adaptive-fill-mode) t)
  (set (make-local-variable 'adaptive-fill-regexp)
       "[ \t]*\\(//+\\|\\**\\)[ \t]*\\([ \t]*\\([-|#;>*]+[ \t]*\\|(?[0-9]+[.)][ \t]*\\)*\\)")
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
  "Major mode for editing SuperCollider language code.
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
  (sclang-init-document)
  (sclang-make-document)
  (run-hooks 'sclang-mode-hook))

;; =====================================================================
;; module initialization
;; =====================================================================

(add-to-list 'auto-mode-alist '("\\.\\(sc\\|scd\\)$" . sclang-mode))
(add-to-list 'interpreter-mode-alist '("sclang" . sclang-mode))

(add-hook 'sclang-library-startup-hook 'sclang-document-library-startup-hook-function)
(add-hook 'kill-buffer-hook 'sclang-document-kill-buffer-hook-function)
(add-hook 'post-command-hook 'sclang-document-post-command-hook-function)
(add-hook 'change-major-mode-hook 'sclang-document-change-major-mode-hook-function)

(provide 'sclang-mode)

;; EOF
