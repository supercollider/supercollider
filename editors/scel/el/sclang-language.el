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
  (require 'cl))

(require 'sclang-browser)
(require 'sclang-interp)
(require 'sclang-util)

;; =====================================================================
;; regexp utilities
;; =====================================================================

(defun sclang-regexp-group (regexp &optional addressable)
  "Enclose REGEXP in grouping parentheses.

If ADDRESSABLE is non-nil the group match data can be addressed
separately after matching."
  (concat "\\(" (unless addressable "?:") regexp "\\)"))

(defun sclang-regexp-concat (&rest regexps)
  "Concatenate REGEXPS by grouping.

The expressions are joined as alternatives with the \\| operator."
  (mapconcat 'sclang-regexp-group regexps "\\|"))

;; =====================================================================
;; some useful regular expressions
;; =====================================================================

(defconst sclang-symbol-regexp
  "\\(?:\\sw\\|\\s_\\)*"
  "Regular expression matching symbols.")

(defconst sclang-identifier-regexp
  (concat "[a-zA-Z]" sclang-symbol-regexp)
  "Regular expression matching valid identifiers.")

(defconst sclang-method-name-special-chars
  "-!%&*+/<=>?@|"
  "Regular expression matching special method name characters.")

(defconst sclang-method-name-plain-regexp
  (concat sclang-identifier-regexp "_?")
  "Regular expression matching regular method names.")

(defconst sclang-method-name-special-regexp
  (concat
   "[" (regexp-quote sclang-method-name-special-chars) "]+")
   "Regular expression matching method names composed of special characters.")

(defconst sclang-method-name-regexp
  (sclang-regexp-concat
   sclang-method-name-special-regexp
   sclang-method-name-plain-regexp)
  "Regular expression matching method names.")

(defconst sclang-class-name-regexp
  "\\<\\(?:Meta_\\)?[A-Z]\\(?:\\sw\\|\\s_\\)*"
  "Regular expression matching class names.")

(defconst sclang-primitive-name-regexp
  (concat "_[A-Z]" sclang-symbol-regexp)
  "Regular expression matching primitive names.")

(defconst sclang-symbol-name-regexp
  (sclang-regexp-concat
   sclang-method-name-regexp
   sclang-class-name-regexp)
  "Regular expression matching class and method names.")

(defconst sclang-class-definition-regexp
  (concat "^\\s *\\("
	  sclang-class-name-regexp
	  "\\)\\(?:\\s *:\\s *\\("
	  sclang-class-name-regexp
	  "\\)\\)?[[:space:]]*{")
  "Regular expression matching class definitions.")

(defconst sclang-method-definition-regexp
  (concat "^\\s *\\*?\\(" sclang-method-name-regexp "\\)\\s *{")
  "Regular expression matching method definitions.")

(defconst sclang-block-regexp
  "^\\((\\)\\s *\\(?:/[/*]?.*\\)?"
  "Regular expression matching the beginning of a code block.

A block is enclosed by parentheses where the opening parenthesis must
be at the beginning of a line to avoid ambiguities.")

(defconst sclang-beginning-of-defun-regexp
    (sclang-regexp-concat
     sclang-class-definition-regexp
     sclang-block-regexp)
    "Regular expression matching the beginning of defuns.

The match is either the start of a class definition
\(`sclang-class-definition-regexp') or the beginning of a code block
enclosed by parenthesis (`sclang-block-regexp').")

(defconst sclang-method-definition-spec-regexp
  (concat (sclang-regexp-group sclang-class-name-regexp t)
	  "-"
	  (sclang-regexp-group sclang-method-name-regexp t))
  "Regular expression matching definition specifications.

A specification is of the form <class-name>-<method-name>.")

;; =====================================================================
;; regexp building
;; =====================================================================

(defun sclang-make-class-definition-regexp (name)
  "Return a regular expression matching the class definition NAME."
  (concat "\\(" (regexp-quote name) "\\)"
	  "\\(?:\\s *:\\s *\\(" sclang-class-name-regexp "\\)\\)?"
	  "[[:space:]]*{"))

(defun sclang-make-class-extension-regexp (name)
  "Return a regular expression matching the class extension NAME."
  (concat "\\+\\s *\\(" (regexp-quote name) "\\)"
	  "\\s *{"))

(defun sclang-make-method-definition-regexp (name)
  "Return a regular expression matching the method definition NAME."
  (concat "\\(" (regexp-quote name) "\\)\\s *{"))

;; =====================================================================
;; string matching
;; =====================================================================

(defun sclang-string-match (regexp string)
  "Match REGEXP with STRING while preserving case."
  (let ((case-fold-search nil))
    (string-match regexp string)))

(defun sclang-symbol-match (symbol-regexp string)
  (sclang-string-match (concat "^" symbol-regexp "$") string))

;; =====================================================================
;; symbol name predicates
;; =====================================================================

(defun sclang-class-name-p (string)
  (sclang-symbol-match sclang-class-name-regexp string))

(defun sclang-meta-class-name-p (string)
  (and (sclang-class-name-p string)
       (sclang-string-match "^Meta_" string)))

(defun sclang-method-name-p (string)
  (sclang-symbol-match sclang-method-name-regexp string))

(defun sclang-symbol-name-p (string)
  (sclang-symbol-match sclang-symbol-name-regexp string))

(defun sclang-method-name-setter-p (method-name)
  (string-match "_$" method-name))

(defun sclang-method-name-getter-p (method-name)
  (not (sclang-method-name-setter-p method-name)))

;; =====================================================================
;; symbol name manipulation
;; =====================================================================

(defun sclang-method-name-setter (method-name)
  (if (sclang-method-name-setter-p method-name)
      method-name
    (concat method-name "_")))

(defun sclang-method-name-getter (method-name)
  (if (sclang-method-name-setter-p method-name)
      (substring method-name 0 (1- (length method-name)))
    method-name))

;; =====================================================================
;; symbol table access
;; =====================================================================

(defcustom sclang-use-symbol-table t
  "*Retrieve symbol table upon library initialization.

Symbol table retrieval is performed each time the library is
recompiled. This takes some time and the symbol table has to be held
in memory, so it might be necessary to disable this option on
low-resource systems."
  :group 'sclang-interface
  :version "21.3"
  :type 'boolean)

(defvar sclang-symbol-table nil
  "List of all defined symbols.")

(defvar sclang-symbol-history nil
  "List of recent symbols read from the minibuffer.")

(defvar sclang-symbol-table-file nil)

(sclang-set-command-handler
 'symbolTable
 (lambda (arg)
   (when (and sclang-use-symbol-table arg)
     (setq sclang-symbol-table (sort arg 'string<))
     (sclang-update-font-lock))))

(add-hook 'sclang-library-startup-hook
	  (lambda ()
	    (when sclang-use-symbol-table
	      (let ((file (make-temp-file "sclang-symbol-table.")))
		(when (and file (file-exists-p file))
		  (setq sclang-symbol-table-file file)
		  (sclang-perform-command 'symbolTable file))))))

(add-hook 'sclang-library-shutdown-hook
	  (lambda ()
	    (setq sclang-symbol-table nil)
	    (sclang-update-font-lock)))

(defun sclang-get-symbol-completion-table ()
  (mapcar (lambda (s) (cons s nil)) sclang-symbol-table))

(defun sclang-make-symbol-completion-predicate (predicate)
  (and predicate (lambda (assoc) (funcall predicate (car assoc)))))

(defun sclang-get-symbol (string)
  (if (and sclang-use-symbol-table sclang-symbol-table)
      (car (member string sclang-symbol-table))
    string))

(defun sclang-read-symbol (prompt &optional default predicate require-match inherit-input-method)
  (if sclang-use-symbol-table
      (flet ((make-minibuffer-local-map
	      (parent-keymap)
	      (let ((map (make-sparse-keymap)))
		(set-keymap-parent map parent-keymap)
		;; override keys bound to valid symbols
		(define-key map [??] 'self-insert-command)
		map)))
	(let ((symbol (sclang-get-symbol default))
	      (minibuffer-local-completion-map (make-minibuffer-local-map
						minibuffer-local-completion-map))
	      (minibuffer-local-must-match-map (make-minibuffer-local-map
						minibuffer-local-completion-map)))
	  (completing-read (sclang-make-prompt-string prompt symbol)
			   (sclang-get-symbol-completion-table)
			   (sclang-make-symbol-completion-predicate predicate)
			   require-match nil
			   'sclang-symbol-history symbol
			   inherit-input-method)))
    (read-string (sclang-make-prompt-string prompt default) nil
		 'sclang-symbol-history default inherit-input-method)))

;; =====================================================================
;; buffer movement
;; =====================================================================

(defun sclang-point-in-comment-p ()
  "Return non-nil if point is inside a comment.

Use font-lock information if font-lock-mode is enabled."
  (if (and (boundp 'font-lock-mode) (eval 'font-lock-mode))
      ;; use available information in font-lock-mode
      (eq (get-text-property (point) 'face) 'font-lock-comment-face)
    ;; else parse from the beginning
    (save-excursion
      (let ((beg (point)))
	(beginning-of-defun)
	(not (null (nth 4 (parse-partial-sexp (point) beg))))))))

(defun sclang-beginning-of-defun (&optional arg)
  (interactive "p")
  (let ((case-fold-search nil)
	(arg (or arg (prefix-numeric-value current-prefix-arg)))
	(orig (point))
	(success t))
    (while (and success (> arg 0))
      (setq success (re-search-backward sclang-beginning-of-defun-regexp
					nil 'move))
      (when (and success (not (sclang-point-in-comment-p)))
	(goto-char (match-beginning 0))
	(setq arg (1- arg))))
    (while (and success (< arg 0))
      (setq success (re-search-forward sclang-beginning-of-defun-regexp nil t))
      (when (and success (not (sclang-point-in-comment-p)))
	(goto-char (match-end 0))
	(setq arg (1+ arg))))
    (when success
      (beginning-of-line)
      (cond ((looking-at sclang-block-regexp) (goto-char (1- (match-end 1))))
	    ((looking-at sclang-class-definition-regexp) (goto-char (1- (match-end 0)))))
      t)))

(defun sclang-point-in-defun-p ()
  "Return non-nil if point is inside a defun.
Return value is nil or (beg end) of defun."
  (save-excursion
    (let ((orig (point))
	  beg end)
      (and (progn (end-of-line) (beginning-of-defun-raw 1) t)
	   (setq beg (point))
	   (condition-case nil (forward-list 1) (error nil))
	   (setq end (point))
	   (list beg end)))))

(defun sclang-end-of-defun (&optional arg)
  (interactive "p")
  (let ((case-fold-search nil)
	(arg (or arg (prefix-numeric-value current-prefix-arg)))
	(success t)
	n cur)
    (while (and success (> arg 0))
      (setq n (if (sclang-point-in-defun-p) 1 -1))
      (setq cur (point))
      (if (and (sclang-beginning-of-defun n)
	       (condition-case nil (forward-list 1) (error nil)))
	  (progn
	    (setq arg (1- arg)))
	(goto-char cur)
	(setq success nil)))
    (while (and success (< arg 0))
      (setq n (if (sclang-point-in-defun-p) 2 1))
      (setq cur (point))
      (if (and (sclang-beginning-of-defun n)
	       (condition-case nil (forward-list 1) (error nil)))
	  (progn
	    (backward-char 1)
	    (setq arg (1+ arg)))
	(goto-char cur)
	(setq success nil)))
    (when success
      (forward-line 1) t)))

;; =====================================================================
;; buffer object access
;; =====================================================================

(defun sclang-symbol-at-point (&optional symbol-name-regexp)
  "Return the symbol at point, or nil if not a valid symbol.

The argument SYMBOL-NAME-REGEXP can be used to specify the type of
symbol matched, candidates are `sclang-symbol-name-regexp' and
`sclang-primitive-name-regexp', the default is
`sclang-symbol-name-regexp'."
  (save-excursion
    (with-syntax-table sclang-mode-syntax-table
      (let ((case-fold-search nil)
	    beg end)
	(cond ((looking-at sclang-method-name-special-regexp)
	       (skip-chars-backward sclang-method-name-special-chars)
	       (setq beg (point))
	       (skip-chars-forward sclang-method-name-special-chars)
	       (setq end (point)))
	      (t
	       (skip-syntax-backward "w_")
	       (setq beg (point))
	       (skip-syntax-forward "w_")
	       (setq end (point))))
	(goto-char beg)
	(if (looking-at (or symbol-name-regexp sclang-symbol-name-regexp))
	    (buffer-substring-no-properties beg end))))))

(defun sclang-line-at-point ()
  "Return the line at point."
  (buffer-substring-no-properties (line-beginning-position) (line-end-position)))

(defun sclang-defun-at-point ()
  "Return the defun at point.

A defun may either be a class definition or a code block, see
`sclang-beginning-of-defun-regexp'."
  (save-excursion
    (with-syntax-table sclang-mode-syntax-table
      (multiple-value-bind (beg end) (sclang-point-in-defun-p)
	(and beg end (buffer-substring-no-properties beg end))))))

;; =====================================================================
;; symbol completion
;; =====================================================================

(defun sclang-complete-symbol (&optional predicate)
  "Perform completion on symbol preceding point.
Compare that symbol against the known symbols.

When called from a program, optional arg PREDICATE is a predicate
determining which symbols are considered.
If PREDICATE is nil, the context determines which symbols are
considered.  If the symbol starts with an upper case letter,
class name completion is performed, otherwise only selector names
are considered."
  (interactive)
  (let* ((buffer (current-buffer))
	 (end (point))
	 (beg (save-excursion
		(backward-sexp 1)
		(skip-syntax-forward "'")
		(point)))
	 (pattern (buffer-substring-no-properties beg end))
	 (case-fold-search nil)
	 (table (sclang-get-symbol-completion-table))
	 (predicate (or predicate
			(if (sclang-class-name-p pattern)
			    'sclang-class-name-p
			  'sclang-method-name-p)))
	 (completion (try-completion pattern table (lambda (assoc) (funcall predicate (car assoc))))))
    (cond ((eq completion t))
	  ((null completion)
	   (sclang-message "Can't find completion for '%s'" pattern)
	   (ding))
	  ((not (string= pattern completion))
	   (delete-region beg end)
	   (insert completion))
	  (t
	   (sclang-message "Making completion list...")
	   (let* ((list (all-completions pattern table (lambda (assoc) (funcall predicate (car assoc)))))
		  (win (selected-window))
		  (buffer-name (sclang-make-buffer-name "Completions"))
		  (same-window-buffer-names (list buffer-name)))
	     (setq list (sort list 'string<))
	     (with-sclang-browser
	      buffer-name
	      (add-hook 'sclang-browser-show-hook (lambda () (sclang-browser-next-link)))
	      (setq sclang-browser-link-function
		    (lambda (arg)
		      (sclang-browser-quit)
		      (with-current-buffer (car arg)
			(delete-region (car (cdr arg)) (point))
			(insert (cdr (cdr arg))))))
	      ;; (setq view-exit-action 'kill-buffer)
	      (insert (format "Completions for '%s':\n\n" pattern))
	      (dolist (x list)
		(insert (sclang-browser-make-link x (cons buffer (cons beg x))))
		(insert " \n"))))
	   (sclang-message "Making completion list...%s" "done")))))

;; =====================================================================
;; introspection
;; =====================================================================

(defcustom sclang-definition-marker-ring-length 32
  "*Length of marker ring `sclang-definition-marker-ring'."
  :group 'sclang-interface
  :version "21.3"
  :type 'integer)

(defvar sclang-definition-marker-ring
  (make-ring sclang-definition-marker-ring-length)
  "Ring of markers which are locations from which \\[sclang-find-definitions] was invoked.")

;; really do that?
(add-hook 'sclang-library-startup-hook
	  (lambda ()
	    (setq sclang-definition-marker-ring
		  (make-ring sclang-definition-marker-ring-length))))

(defun sclang-open-definition (name file pos &optional pos-func)
  (let ((buffer (find-file file)))
    (when (bufferp buffer)
      (with-current-buffer buffer
	(goto-char (or pos (point-min)))
	(when (and nil (functionp pos-func))
	  (let ((pos (funcall pos-func name)))
	    (and pos (goto-char pos))))))
    buffer))

(defun sclang-pop-definition-mark ()
  "Pop back to where \\[sclang-find-definition] or \\[sclang-find-reference] was last invoked."
  (interactive)
  (unless (ring-empty-p sclang-definition-marker-ring)
    (let ((marker (ring-remove sclang-definition-marker-ring 0)))
      (switch-to-buffer (or (marker-buffer marker)
			    (error "The marked buffer has been deleted")))
      (goto-char (marker-position marker))
      (set-marker marker nil nil))))

(defun sclang-browse-definitions (name definitions buffer-name header &optional pos-func)
  (if (cdr definitions)
      (let ((same-window-buffer-names (list buffer-name)))
	(with-sclang-browser
	 buffer-name
	 ;; (setq view-exit-action 'kill-buffer)
	 (setq sclang-browser-link-function
	       (lambda (data)
		 (sclang-browser-quit)
		 (apply 'sclang-open-definition data)))
	 (add-hook 'sclang-browser-show-hook (lambda () (sclang-browser-next-link)))
	 (insert header)
	 (insert "\n")
	 (let ((max-width 0)
	       format-string)
	   (dolist (def definitions)
	     (setq max-width (max (length (file-name-nondirectory (nth 1 def))) max-width)))
	   (setq format-string (format "%%-%ds  %%s" max-width))
	   (dolist (def definitions)
	     (let ((string (format format-string
				   (propertize (file-name-nondirectory (nth 1 def)) 'face 'bold)
				   (nth 0 def)))
		   (data (list name (nth 1 def) (nth 2 def) pos-func)))
	       (insert (sclang-browser-make-link string data))
	       (insert "\n"))))))
    ;; single definition: jump directly
    (let ((def (car definitions)))
      (sclang-open-definition name (nth 1 def) (nth 2 def) pos-func))))

(defun sclang-find-definitions (name)
  "Find all definitions of symbol NAME."
  (interactive
   (list
    (if current-prefix-arg
	(read-string "Find definition: ")
      (sclang-read-symbol "Find definitions of: "
			  (sclang-symbol-at-point) nil t))))
  (if (sclang-symbol-match sclang-method-definition-spec-regexp name)
      (sclang-perform-command 'openDefinition name)
    (if (sclang-get-symbol name)
	(progn
	  (ring-insert sclang-definition-marker-ring (point-marker))
	  (if (sclang-class-name-p name)
	      (sclang-perform-command 'classDefinitions name)
	    (sclang-perform-command 'methodDefinitions name)))
      (sclang-message "'%s' is undefined" name))))

(sclang-set-command-handler
 'openDefinition
 (lambda (assoc)
   (let ((name (car assoc))
	 (data (cdr assoc)))
     (if data
	 (sclang-open-definition nil (car data) (cadr data))
       (sclang-message "'%s' is undefined" name)))))

(sclang-set-command-handler
 'classDefinitions
 (lambda (assoc)
   (let ((name (car assoc))
	 (data (cdr assoc)))
     (if data
	 (sclang-browse-definitions
	  name data
	  "*Definitions*" (format "Definitions of '%s'\n" name)
	  (lambda (name)
	    (let ((case-fold-search nil))
	      ;; point is either
	      ;;  - at a class definition
	      ;;  - inside a class extension
	      ;; so jump to the class name
	      (when (or (looking-at (sclang-make-class-definition-regexp name))
			(re-search-backward (sclang-make-class-extension-regexp name) nil t))
		(match-beginning 1)))))
       (sclang-message "No definitions of '%s'" name)))))

(sclang-set-command-handler
 'methodDefinitions
 (lambda (assoc)
   (let ((name (car assoc))
	 (data (cdr assoc)))
     (if data
	 (sclang-browse-definitions
	  name data
	  "*Definitions*" (format "Definitions of '%s'\n" name)
	  (lambda (name)
	    (let ((case-fold-search nil))
	      (when (re-search-forward (sclang-make-method-definition-regexp name))
		(match-beginning 1)))))
       (sclang-message "No definitions of '%s'" name)))))

(defun sclang-find-references (name)
  "Find all references to symbol NAME."
  (interactive
   (list
    (sclang-read-symbol "Find references to: "
			(sclang-symbol-at-point) nil t)))
  (if (sclang-get-symbol name)
      (progn
	(ring-insert sclang-definition-marker-ring (point-marker))
	(sclang-perform-command 'methodReferences name))
    (sclang-message "'%s' is undefined" name)))

(sclang-set-command-handler
 'methodReferences
 (lambda (assoc)
   (let ((name (car assoc))
	 (data (cdr assoc)))
     (if data
	 (sclang-browse-definitions
	  name data
	  "*References*" (format "References to '%s'\n" name)
	  (lambda (name)
	    (let ((case-fold-search nil))
	      (when (re-search-forward (regexp-quote name))
		(match-beginning 0)))))
       (sclang-message "No references to '%s'" name)))))

(defun sclang-show-method-args ()
  "whooha. in full effect."
  (interactive)
  (let ((regexp (concat
		 sclang-class-name-regexp
		 "[ \t\n]*\\(?:\\.[ \t\n]*\\("
		 sclang-method-name-regexp
		 "\\)\\)?[ \t\n]*("))
	(case-fold-search nil)
	(beg (point)))
    (save-excursion
      (while (and (re-search-backward regexp nil t)
		  (let ((class (save-match-data (sclang-get-symbol (sclang-symbol-at-point)))))
		    (goto-char (1- (match-end 0)))
		    (if (condition-case nil
			    (save-excursion
			      (forward-list 1)
			      (> (point) beg))
			  (error t))
			(let ((method (sclang-get-symbol (or (match-string-no-properties 1) "new"))))
			  (and class method
			       (sclang-perform-command 'methodArgs class method)
			       nil))
		      (goto-char (match-beginning 0)) t)))))))

(sclang-set-command-handler
 'methodArgs
 (lambda (args)
   (and args (message "%s" args))))

(defun sclang-dump-full-interface (class)
  "Dump interface of CLASS."
  (interactive
   (list
    (let* ((symbol (sclang-symbol-at-point))
	   (class (and (sclang-get-symbol symbol)
		       (sclang-class-name-p symbol)
		       symbol)))
      (sclang-read-symbol "Dump interface of: "
			  class 'sclang-class-name-p t))))
  (sclang-eval-string (format "%s.dumpFullInterface" class)))

(defun sclang-dump-interface (class)
  "Dump interface of CLASS."
  (interactive
   (list
    (let* ((symbol (sclang-symbol-at-point))
	   (class (and (sclang-get-symbol symbol)
		       (sclang-class-name-p symbol)
		       symbol)))
      (sclang-read-symbol "Dump interface of: "
			  class 'sclang-class-name-p t))))
  (sclang-eval-string (format "%s.dumpInterface" class)))

;; =====================================================================
;; cscope interface
;; =====================================================================

(defcustom sclang-source-directory nil
  "Toplevel SuperCollider source directory.

This variable is used by `sclang-find-primitive' to locate the cscope
database."
  :group 'sclang-interface
  :version "21.4.1"
  :type 'directory
  :options '(must-match))

(defun sclang-find-primitive (name)
  "Find primitive name a cscope database.

The database is searched in `sclang-source-directory', or the
current-directory, iff `sclang-source-directoy' is nil."
  (interactive
   (let ((default (sclang-symbol-at-point sclang-primitive-name-regexp)))
     (list (read-string (sclang-make-prompt-string "Find primitive: " default)
			nil nil default))))
  (if (require 'xcscope nil t)
      (let ((cscope-initial-directory sclang-source-directory))
	(cscope-find-this-text-string
	 (if (string-match "^_" name) name (concat "_" name))))
    (sclang-message "cscope not available")))

;; =====================================================================
;; sc-code formatting
;; =====================================================================

(defun sclang-list-to-string (list)
  (mapconcat 'sclang-object-to-string list ", "))

(defconst false 'false)

(defun sclang-object-to-string (obj)
  (cond ((null obj)
	 "nil")
	((eq false obj)
	 "false")
	((eq t obj)
	 "true")
	((symbolp obj)
	 (format "'%s'" obj))
	((listp obj)
	 (format "[ %s ]" (sclang-list-to-string obj)))
	(t (format "%S" obj))))

(defun sclang-format (string &rest args)
  "format chars:
     %s - print string
     %o - print object
     %l - print argument list"
  (let ((case-fold-search nil)
	(i 0))
    (save-match-data
      (while (and (< i (length string))
		  (string-match "%[los%]" string i))
	(let* ((start (car (match-data)))
	       (format (aref string (1+ start)))
	       (arg (if args
			(pop args)
		      (error "Not enough arguments for format string")))
	       (repl (cond ((eq ?o format)
			    (sclang-object-to-string arg))
			   ((eq ?l format)
			    (if (listp arg)
				(sclang-list-to-string arg)
			      (sclang-object-to-string arg)))
			   ((eq ?s format)
			    (format "%s" arg))
			   ((eq ?% format)
			    (push arg args)
			    "%"))))
	  (setq string (replace-match repl t t string))
	  (setq i (+ start (length repl)))))))
  string)

(defun sclang-format-pseq (items)
  "Format ITEMS (a flat list of numbers or symbols) as a possibly nested Pseq.
Looks for all repetitive patterns in ITEMS recursively.  Therefore, it is
computationally expensive, especially when ITEMS is a long list.  If you don't
want smart pattern guessing, use `sclang-format' directly to format your Pseq."
  (flet ((find-reps (items)
	   (let (r)
	     (while items
	       (let ((ret (car items))
		     (skip 1)
		     (rep (length items)))
		 (catch 'match-found
		   (while (>= rep 2)
		     (let ((i (/ (length items) rep)))
		       (while (> i 0)
			 (let ((sublst (subseq items 0 i)))
			   (when (catch 'equal
				   (let ((a items))
				     (loop repeat rep do
					   (let ((b sublst))
					     (while b
					       (unless (eql (car b) (car a))
						 (throw 'equal nil))
					       (setq a (cdr a)
						     b (cdr b)))))
				     t))
			     (setq ret (cons rep (if (> i 5)
						     (find-reps sublst)
						   sublst))
				   skip (* i rep))
			     (throw 'match-found t))
			   (decf i))))
		     (decf rep)))
		 (accept-process-output nil 0 100)
		 (message "Processed...%S" ret) ;; invent better progress info
		 (setq r (append r (list ret))
		       items (nthcdr skip items))))
	     r))
	 (elem-to-string (elem)
	   (cond
	    ((consp elem)
	     (concat "Pseq([ "
		     (mapconcat #'elem-to-string (cdr elem) ", ")
		     (format " ], %d)" (car elem))))
	    (t (sclang-object-to-string elem)))))
    (let ((compressed (find-reps items)))
      (if (and (= (length compressed) 1) (consp (car compressed)))
	  (elem-to-string (car compressed))
	(concat "Pseq([ "
		(mapconcat #'elem-to-string compressed ", ")
		" ], 1)")))))

;; =====================================================================
;; module setup
;; =====================================================================

(provide 'sclang-language)

;; EOF
