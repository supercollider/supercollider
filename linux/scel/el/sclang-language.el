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

(defun sclang-regexp-group (regexp)
  (concat "\\(" regexp "\\)"))

(defun sclang-regexp-concat (&rest regexps)
  (mapconcat 'sclang-regexp-group regexps "\\|"))

(defconst sclang-symbol-regexp "\\(\\sw\\|\\s_\\)*")
(defconst sclang-identifier-regexp (concat "[a-z]" sclang-symbol-regexp))

(defconst sclang-method-name-special-chars "-!%&*+/<=>?@|")

(defconst sclang-method-name-plain-regexp (concat sclang-identifier-regexp "_?"))

(defconst sclang-method-name-special-regexp (concat
					     "[" (regexp-quote sclang-method-name-special-chars) "]+"
					     ))

(defconst sclang-method-name-regexp (sclang-regexp-concat
				     sclang-method-name-special-regexp
				     sclang-method-name-plain-regexp
				     ))

(defconst sclang-class-name-regexp "\\(Meta_\\)?[A-Z]\\(\\sw\\|\\s_\\)*")

(defconst sclang-symbol-name-regexp (sclang-regexp-concat
				     sclang-method-name-regexp
				     sclang-class-name-regexp
				     ))

(defconst sclang-class-definition-regexp (concat
					  "^\\s *"
					  (sclang-regexp-group sclang-class-name-regexp)
					  "\\(\\s *:\\s *"
					  (sclang-regexp-group sclang-class-name-regexp)
					  "\\)?\\s *{"))

(defconst sclang-method-definition-regexp (concat
					   "^\\s *\\*?"
					   (sclang-regexp-group sclang-method-name-regexp)
					   "\\s *{"))

(defconst sclang-beginning-of-defun-regexp (concat
					    "^"
					    (sclang-regexp-group
					     (sclang-regexp-concat
					      "\\s("
					      sclang-class-definition-regexp
;; 					      sclang-method-definition-regexp
					      ))))

(defvar sclang-symbol-table nil
  "Obarray of all defined symbols.")

(defvar sclang-symbol-history nil)

;; =====================================================================
;; regexp building
;; =====================================================================

;; (defun sclang-make-class-definition-regexp (name)
;;   (concat
;;    "\\(" (regexp-quote name) "\\)"
;;    "\\(\\s *:\\s *\\(" sclang-class-name-regexp "\\)\\)?"
;;    "\\s *{"))

;; (defun sclang-make-method-definition-regexp (name)
;;   (concat "\\(" (regexp-quote name) "\\)\\s *{"))

;; =====================================================================
;; string matching
;; =====================================================================

(defun sclang-string-match (regexp string)
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
       (match-string 1 string)))

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

(sclang-set-command-handler
 'symbolTable
 (lambda (arg)
   (let* ((size (car arg))
	  (data (cdr arg))
	  (table (make-vector size nil)))
     (dolist (name data)
       (let ((sym (intern name table)))
	 ;; store various symbol information
	 (put sym 'type
	      (cond ((sclang-meta-class-name-p name) 'meta-class)
		    ((sclang-class-name-p name) 'class)
		    ((sclang-method-name-p name) 'method)))))
     (setq sclang-symbol-table table)
     (sclang-update-font-lock))))

(add-hook 'sclang-library-startup-hook
	  (lambda () (sclang-perform-command 'symbolTable)))

(defun sclang-get-symbol (symbol-name)
  (flet ((get (s) (intern-soft s sclang-symbol-table)))
    (if (sclang-class-name-p symbol-name)
	(get symbol-name)
      (or (get (sclang-method-name-getter symbol-name))
	  (get (sclang-method-name-setter symbol-name))))))

(defun sclang-meta-class-p (symbol)
  (eq (get symbol 'type) 'meta-class))

(defun sclang-class-p (symbol)
  (or (sclang-meta-class-p symbol)
      (eq (get symbol 'type) 'class)))

(defun sclang-method-p (symbol)
  (eq (get symbol 'type) 'method))

(defun sclang-read-symbol (prompt &optional default predicate require-match inherit-input-method)
  (let ((symbol (and default (sclang-get-symbol default))))
    (completing-read (format prompt
			     (if symbol (format " (default %s)" default) ""))
		     sclang-symbol-table predicate
		     require-match nil
		     'sclang-symbol-history symbol)))

;; =====================================================================
;; buffer movement
;; =====================================================================

(defun sclang-point-in-comment-p ()
  "Return non-nil if point is inside a comment.

Use font-lock information is font-lock-mode is enabled."
  (if font-lock-mode
      ;; use available information in font-lock-mode
      (eq (get-text-property (point) 'face) 'font-lock-comment-face)
    ;; else parse from the beginning
    (not (null (nth 4 (parse-partial-sexp (point-min) (point)))))))

(defun sclang-beginning-of-defun (&optional arg)
  (interactive "p")
  (let ((case-fold-search nil)
	(arg (or arg (prefix-numeric-value current-prefix-arg)))
	(orig (point))
	(success t))
    (while (and success (> arg 0))
      (if (setq success (re-search-backward sclang-beginning-of-defun-regexp nil t))
	  (unless (sclang-point-in-comment-p)
	    (goto-char (match-beginning 0))
	    (setq arg (1- arg)))))
    (while (and success (< arg 0))
      (if (setq success (re-search-forward sclang-beginning-of-defun-regexp nil t))
	  (unless (sclang-point-in-comment-p)
	    (goto-char (match-end 0))
	    (setq arg (1+ arg)))))
    (when success
	(beginning-of-line) t)))

(defun sclang-point-in-defun-p ()
  "Return non-nil if point is inside a defun.
Return value is nil or (values beg end) of defun."
  (save-excursion
    (let ((orig (point))
	  beg end)
      (and (sclang-beginning-of-defun 1)
	   (setq beg (point))
	   (condition-case nil (forward-list 1) (error nil))
	   (setq end (point))
	   (> (point) orig)
	   (values beg end)))))

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

;; (defun sclang-thing-at-point (thing)
;;   (with-syntax-table sclang-mode-syntax-table
;;     (let ((bounds (bounds-of-thing-at-point thing)))
;;       (and bounds
;; 	   (buffer-substring-no-properties (car bounds) (cdr bounds))))))

(defun sclang-symbol-at-point ()
  "Answer the symbol at point, or nil if not a valid symbol."
  (save-excursion
    (with-syntax-table sclang-mode-syntax-table
      (let (beg end)
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
	(if (looking-at sclang-method-name-regexp)
	    (buffer-substring-no-properties beg end))))))

(defun sclang-line-at-point ()
  (let (beg end)
    (save-excursion
      (beginning-of-line)
      (setq beg (point))
      (end-of-line)
      (setq end (point)))
    (unless (eq beg end)
      (buffer-substring-no-properties beg end))))

(defun sclang-defun-at-point ()
  (save-excursion
    (with-syntax-table sclang-mode-syntax-table
      (multiple-value-bind
	  (beg end) (sclang-point-in-defun-p)
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
	 (predicate (or predicate
			(if (sclang-class-name-p pattern)
			    'sclang-class-p
			  'sclang-method-p)))
			
	 (completion (try-completion pattern sclang-symbol-table predicate)))
    (cond ((eq completion t))
	  ((null completion)
	   (message "Can't find completion for '%s'" pattern)
	   (ding))
	  ((not (string= pattern completion))
	   (delete-region beg end)
	   (insert completion))
	  (t
	   (message "Making completion list...")
	   (let* ((list (all-completions pattern sclang-symbol-table predicate))
		  (win (selected-window))
		  (buffer-name "*Completions*")
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
		(insert " \n"))
	      ))
	   (message "Making completion list...%s" "done")))))

;; =====================================================================
;; browsing definitions
;; =====================================================================

(defcustom sclang-symbol-definition-marker-ring-length 32
  "*Length of marker ring `sclang-symbol-definition-marker-ring'."
  :group 'sclang-interface
  :version "21.3"
  :type 'integer)

(defvar sclang-symbol-definition-marker-ring
  (make-ring sclang-symbol-definition-marker-ring-length)
  "Ring of markers which are locations from which \\[sclang-find-symbol-definitions] was invoked.")

;; symbol definition: (key . (file . pos))
;; TODO: revise all this stuff ...

(sclang-set-command-handler
 'symbolDefinitions
 (lambda (arg)
   (let* ((name (car arg))
	  (data (cdr arg))
	  (symbol (sclang-get-symbol name)))
     (when (and (symbolp symbol) (consp data))
       ;; cache definitions; turn key string into symbol
       (put symbol 'definitions (mapcar (lambda (def)
					  (cons (sclang-get-symbol (car def))
						(cdr def)))
					data))
       (sclang-display-symbol-definition-list symbol)))))

(add-hook 'sclang-library-startup-hook
	  (lambda ()
	    (setq sclang-symbol-definition-marker-ring
		  (make-ring sclang-symbol-definition-marker-ring-length))))

(defun sclang-get-symbol-definitions (symbol)
  (get symbol 'definitions))

(defun sclang-symbol-definition (symbol key)
  (assq key (sclang-get-symbol-definitions symbol)))

(defun sclang-symbol-definition-key (def)
  (car def))

(defun sclang-symbol-definition-file (def)
  (car (cdr def)))

(defun sclang-symbol-definition-pos (def)
  (cdr (cdr def)))

(defun sclang-open-symbol-definition (symbol def)
  (let* ((key (sclang-symbol-definition-key def))
	 (file (sclang-symbol-definition-file def))
	 (pos (sclang-symbol-definition-pos def))
	 (regexp (sclang-regexp-group (regexp-quote (symbol-name key))))
	 (buffer (find-file file)))
    (with-current-buffer buffer
      (goto-char pos)
;;       (back-to-indentation)
;;       ;; skip classvar, classmethod clutter
;;       (if (re-search-forward regexp (line-end-position) t)
;; 	  (goto-char (match-beginning 0)))
      buffer)))

(defun sclang-pop-symbol-definition-mark ()
  (interactive)
  (let ((find-tag-marker-ring sclang-symbol-definition-marker-ring))
    (pop-tag-mark)))

(defun sclang--symbol-definition-string (symbol def max-width)
  (let* ((key (sclang-symbol-definition-key def))
	 (file (sclang-symbol-definition-file def))
	 (file-string (propertize (file-name-nondirectory file)
				  'face 'bold))
	 (def-string (cond ((sclang-class-p symbol)
			    (if (eq key symbol)
				(format "  %s" (symbol-name symbol))
			      (format "+ %s-%s" (symbol-name symbol) (symbol-name key))))
			    ((sclang-method-p symbol)
			     (format "%s-%s" (symbol-name key) (symbol-name symbol)))
			    (t
			     (symbol-name symbol)))))
    (format (format "%%-%ds  %%s" max-width) file-string def-string)))

(defun sclang-display-symbol-definition-list (symbol)
  (let* ((symbol-name (symbol-name symbol))
	 (def-list (sclang-get-symbol-definitions symbol))
	 (buffer-name "*Definition Browser*")
	 (same-window-buffer-names (list buffer-name)))
    (cond ((null def-list)
	   (message "No definitions of '%s'" symbol-name))
	  ((cdr def-list)
	   (with-sclang-browser
	    buffer-name
	    ;; (setq view-exit-action 'kill-buffer)
	    (setq sclang-browser-link-function
		  (lambda (arg)
		    (sclang-browser-quit)
		    (apply 'sclang-open-symbol-definition arg)))
	    (add-hook 'sclang-browser-show-hook (lambda () (sclang-browser-next-link)))
	    (insert (format "Definitions of '%s':\n\n" symbol-name))
	    (let ((max-width 0))
	      (dolist (def def-list)
		(setq max-width (max (length (file-name-nondirectory (sclang-symbol-definition-file def)))
				     max-width)))
	      (dolist (def def-list)
		(let* ((link-string (sclang--symbol-definition-string symbol def max-width))
		       (link-data (list symbol def)))
		  (insert (sclang-browser-make-link link-string link-data))
		  (insert "\n"))))))
	  (t
	   ;; single definition: jump directly
	   (sclang-open-symbol-definition symbol (car def-list))))))

(defun sclang-find-symbol-definitions (symbol-name)
  "Find all definitions of SYMBOL-NAME."
  (interactive
   (list
    (let ((sym (sclang-symbol-at-point)))
      (if current-prefix-arg
	  (sclang-read-symbol "Find definitions of%s: " sym nil t)
	(unless sym
	  (message "No symbol at point"))
	sym))))
  (when symbol-name
    (let ((symbol (sclang-get-symbol symbol-name)))
      (if symbol
	  (progn
	    (ring-insert sclang-symbol-definition-marker-ring (point-marker))
	    (if (sclang-get-symbol-definitions symbol)
		(sclang-display-symbol-definition-list symbol)
	      (sclang-perform-command 'symbolDefinitions symbol-name)))
	(message "'%s' is undefined" symbol-name)))))

;; =====================================================================
;; sc-code formatting
;; =====================================================================

(defun sclang-list-to-string (list)
  (unless (listp list) (setq list (list list)))
  (mapconcat 'sclang-object-to-string list ", "))

(defun sclang-object-to-string (obj)
  (cond ((null obj)
	 "nil")
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
     %l - print list"
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
			    (sclang-list-to-string arg))
			   ((eq ?s format)
			    (format "%s" arg))
			   ((eq ?% format)
			    (push arg args)
			    "%"))))
	  (setq string (replace-match repl t t string))
	  (setq i (+ start (length repl)))))))
  string)

;; =====================================================================
;; module setup
;; =====================================================================

(provide 'sclang-language)

;; EOF