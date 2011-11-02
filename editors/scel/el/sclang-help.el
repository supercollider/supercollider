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
;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
;; USA

(eval-when-compile
  (require 'cl)
  (require 'font-lock))

;; (require 'w3m) ;; not needed during compilation
(require 'sclang-util)
(require 'sclang-interp)
(require 'sclang-language)
(require 'sclang-mode)
(require 'sclang-vars)
(require 'sclang-minor-mode)

(defcustom sclang-help-directory "~/SuperCollider/Help"
  "*Directory where the SuperCollider help files are kept. OBSOLETE."
  :group 'sclang-interface
  :version "21.3"
  :type 'directory
  :options '(:must-match))

(defcustom sclang-help-path (list sclang-system-help-dir
				  "~/.local/share/SuperCollider/Help")
  "*List of directories where SuperCollider help files are kept."
  :group 'sclang-interface
  :version "21.4"
  :type '(repeat directory))

(defconst sclang-extension-path (list sclang-system-extension-dir
				      "~/.local/share/SuperCollider/Extensions")
  "List of SuperCollider extension directories.")

(defcustom sclang-help-fill-column fill-column
  "*Column beyond which automatic line-wrapping in RTF help files should happen."
  :group 'sclang-interface
  :version "21.3"
  :type 'integer)

(defcustom sclang-rtf-editor-program "ted"
  "*Name of an RTF editor program used to edit SuperCollider help files."
  :group 'sclang-programs
  :version "21.3"
  :type 'string)

(defcustom sclang-html-editor-program "html"
  "*Name of an HTML editor program used to edit SuperCollider help files."
  :group 'sclang-programs
  :version "21.3"
  :type 'string)

;; dynamically change certain html-tags when displaying in w3m-browser:

(defcustom sclang-help-filters
  '(("p\\.p\\([0-9]+\\)" . "#p\\1")
    ("<p class=\"\\(.*\\)\">\\(.*\\)</p>" . "<div id=\"\\1\">\\2</div>"))
  "list of pairs of (regexp . filter) defining html-tags to be replaced by the function sclang-help-substitute-for-filters"
  :group 'sclang-interface
  :type '(repeat (cons (string :tag "match") (string :tag "replacement"))))

(defun sclang-help-substitute-for-filters (&rest args)
  "substitute various tags in SCs html-docs"
  (mapcar #'(lambda (filter)
	      (let ((regexp (car filter))
		    (to-string (cdr filter)))
		(goto-char (point-min))
		(while (re-search-forward regexp nil t)
		  (replace-match to-string nil nil))))
	  sclang-help-filters))

;; w3m's content-filtering system
(setq w3m-use-filter t)

(eval-after-load "w3m-filter"
  '(add-to-list 'w3m-filter-rules
		;; run on all files read by w3m...
                '(".*" sclang-help-substitute-for-filters)))


(defvar sclang-help-topic-alist nil
  "Alist mapping help topics to file names.")

(defvar sclang-help-topic-history nil
  "List of recently invoked help topics.")
;; (defvar sclang-help-topic-ring-length 32)
;; (defvar sclang-help-topic-ring (make-ring sclang-help-topic-ring-length))

(defconst sclang-special-help-topics
  '(("/" . "division")
    ("-" . "subtraction"))
  "Alist of help topics with transcoded filenames.")

(defvar sclang-help-file nil)
(defvar sclang-current-help-file nil)
(make-variable-buffer-local 'sclang-help-file)

(defconst sclang-help-file-regexp
  "\\(\\(\\(\\.help\\)?\\.\\(rtf\\|scd\\|html\\|htm\\)\\)\\|\\(\\.help\\.sc\\.html\\.htm\\)\\|\\.rtfd/TXT\\.rtf\\.html\\.htm\\)$"
  "Regular expression matching help files.")

;; =====================================================================
;; utilities
;; =====================================================================

(defun sclang-get-help-file (topic)
  (let ((topic (or (cdr (assoc topic sclang-special-help-topics)) topic)))
    (cdr (assoc topic sclang-help-topic-alist))))

(defun sclang-get-help-topic (file)
  (let ((topic (car (rassoc file sclang-help-topic-alist))))
    (or (car (rassoc topic sclang-special-help-topics)) topic)))

(defun sclang-help-buffer-name (topic)
  (sclang-make-buffer-name (concat "Help:" topic)))

(defun sclang-rtf-file-p (file)
  (let ((case-fold-search t))
    (string-match ".*\\.rtf$" file)))

;; ========= ADDITION for HTML help files
(defun sclang-html-file-p (file)
   (let ((case-fold-search t))
     (string-match ".*\\.html?$" file)))

(defun sclang-sc-file-p (file)
  (let ((case-fold-search t))
    (string-match ".*\\.sc$" file)))

(defun sclang-scd-file-p (file)
  (let ((case-fold-search t))
    (string-match ".*\\.scd$" file)))

(defun sclang-help-file-p (file)
  (string-match sclang-help-file-regexp file))

(defun sclang-help-topic-name (file)
  (if (string-match sclang-help-file-regexp file)
      (cons (file-name-nondirectory (replace-match "" nil nil file 1))
	    file)))

;; =====================================================================
;; rtf parsing
;; =====================================================================

(defconst sclang-rtf-face-change-token "\0")

(defun sclang-fill-rtf-syntax-table (table)
  ;; character quote
  (modify-syntax-entry ?\\ "/" table)
  (modify-syntax-entry ?\" "." table)
  (modify-syntax-entry ?\{ "(" table)
  (modify-syntax-entry ?\} ")" table)
  (modify-syntax-entry ?\( "." table)
  (modify-syntax-entry ?\) "." table)
  (modify-syntax-entry ?\[ "." table)
  (modify-syntax-entry ?\] "." table)
  table)

(defvar sclang-rtf-syntax-table (sclang-fill-rtf-syntax-table (make-syntax-table))
  "Syntax table used for RTF parsing.")

(defvar sclang-rtf-font-map '((Helvetica . variable-pitch)
			      (Helvetica-Bold . variable-pitch)
			      (Monaco . nil)))

(defstruct sclang-rtf-state
  output font-table font face pos)

(macrolet ((rtf-p (pos) `(plist-get (text-properties-at ,pos) 'rtf-p)))
  (defun sclang-rtf-p (pos) (rtf-p pos))
  (defun sclang-code-p (pos) (not (rtf-p pos))))

(defmacro with-sclang-rtf-state-output (state &rest body)
  `(with-current-buffer (sclang-rtf-state-output ,state)
     ,@body))

(defmacro sclang-rtf-state-add-font (state font-id font-name)
  `(push (cons ,font-id (intern ,font-name)) (sclang-rtf-state-font-table ,state)))

(defmacro sclang-rtf-state-apply (state)
  (let ((pos (gensym))
	(font (gensym))
	(face (gensym)))
    `(with-current-buffer (sclang-rtf-state-output ,state)
       (let ((,pos (or (sclang-rtf-state-pos ,state) (point-min)))
	     (,font (cdr (assq
			  (cdr (assoc
				(sclang-rtf-state-font ,state)
				(sclang-rtf-state-font-table ,state)))
			  sclang-rtf-font-map)))
	     (,face (sclang-rtf-state-face ,state)))
	 (when (> (point) ,pos)
	   (if ,font
	       (add-text-properties
		,pos (point)
		(list 'rtf-p t 'rtf-face (append (list ,font) ,face))))
	   (setf (sclang-rtf-state-pos ,state) (point)))))))

(defmacro sclang-rtf-state-set-font (state font)
  `(progn
     (sclang-rtf-state-apply ,state)
     (setf (sclang-rtf-state-font ,state) ,font)))

(defmacro sclang-rtf-state-push-face (state face)
  (let ((list (gensym)))
    `(let ((,list (sclang-rtf-state-face state)))
       (sclang-rtf-state-apply ,state)
       (unless (memq ,face ,list)
	 (setf (sclang-rtf-state-face ,state)
	       (append ,list (list ,face)))))))

(defmacro sclang-rtf-state-pop-face (state face)
  (let ((list (gensym)))
    `(let* ((,list (sclang-rtf-state-face ,state)))
       (sclang-rtf-state-apply ,state)
       (setf (sclang-rtf-state-face ,state) (delq ,face ,list)))))

(defun sclang-parse-rtf (state)
  (while (not (eobp))
    (cond ((looking-at "{")
	   ;; container
	   (let ((beg (point)))
	     (with-syntax-table sclang-rtf-syntax-table
	       (forward-list 1))
	     (save-excursion
	       (save-restriction
		 (narrow-to-region (1+ beg) (1- (point)))
		 (goto-char (point-min))
		 (sclang-parse-rtf-container state)
		 (widen)))))
	  ((or (looking-at "\\\\\\([{}\\\n]\\)")
	       (looking-at "\\\\\\([^\\ \n]+\\) ?"))
	   ;; control
	   (let ((end (match-end 0)))
	     (sclang-parse-rtf-control state (match-string 1))
	     (goto-char end)))
	  ((looking-at "\\([^{\\\n]+\\)")
	   ;; normal text
	   (let ((end (match-end 0))
		 (match (match-string 1)))
	     (with-sclang-rtf-state-output state (insert match))
	     (goto-char end)))
	  (t
	   ;; never reached (?)
	   (forward-char 1)))))

(defun sclang-parse-rtf-container (state)
  (cond ((looking-at "\\\\rtf1")		; document
	 (goto-char (match-end 0))
	 (sclang-parse-rtf state))
	((looking-at "\\\\fonttbl")		; font table
	 (goto-char (match-end 0))
	 (while (looking-at "\\\\\\(f[0-9]+\\)[^ ]* \\([^;]*\\);[^\\]*")
	   (sclang-rtf-state-add-font state (match-string 1) (match-string 2))
	   (goto-char (match-end 0))))
	((looking-at "{\\\\NeXTGraphic \\([^\\]+\\.[a-z]+\\)") ; inline graphic
	 (let* ((file (match-string 1))
		(image (and file (create-image (expand-file-name file)))))
	   (with-sclang-rtf-state-output
	    state
	    (if image
		(insert-image image)
	      (sclang-rtf-state-push-face state 'italic)
	      (insert file)
	      (sclang-rtf-state-pop-face state 'italic)))))
	))

(defun sclang-parse-rtf-control (state ctrl)
  (let ((char (aref ctrl 0)))
    (cond ((memq char '(?{ ?} ?\\))
	   (with-sclang-rtf-state-output state (insert char)))
	  ((or (eq char ?\n)
	       (string= ctrl "par"))
	   (sclang-rtf-state-apply state)
	   (with-sclang-rtf-state-output
	    state
	    (when (sclang-rtf-p (line-beginning-position))
	      (fill-region (line-beginning-position) (line-end-position)
			   t t))
	      (insert ?\n)))
	  ((string= ctrl "tab")
	   (with-sclang-rtf-state-output state (insert ?\t)))
	  ((string= ctrl "b")
	   (sclang-rtf-state-push-face state 'bold))
	  ((string= ctrl "b0")
	   (sclang-rtf-state-pop-face state 'bold))
	  ((string-match "^f[0-9]+$" ctrl)
	   (sclang-rtf-state-set-font state ctrl))
	  )))

(defun sclang-convert-rtf-buffer (output)
  (let ((case-fold-search nil)
	(fill-column sclang-help-fill-column))
    (save-excursion
      (goto-char (point-min))
      (when (looking-at "{\\\\rtf1")
	(let ((state (make-sclang-rtf-state)))
	  (setf (sclang-rtf-state-output state) output)
	  (sclang-parse-rtf state)
	  (sclang-rtf-state-apply state))))))

;; =====================================================================
;; help mode
;; =====================================================================

(defun sclang-fill-help-syntax-table (table)
  ;; make ?- be part of symbols for selection and sclang-symbol-at-point
  (modify-syntax-entry ?- "_" table))

(defun sclang-fill-help-mode-map (map)
  (define-key map "\C-c}" 'bury-buffer)
  (define-key map "\C-c\C-v" 'sclang-edit-help-file))

(defmacro sclang-help-mode-limit-point-to-code (&rest body)
  (let ((min (gensym))
	(max (gensym))
	(res (gensym)))
    `(if (and (sclang-code-p (point))
	      (not (or (bobp) (eobp)))
	      (sclang-code-p (1- (point)))
	      (sclang-code-p (1+ (point))))
	 (let ((,min (previous-single-property-change (point) 'rtf-p (current-buffer) (point-min)))
	       (,max (next-single-property-change (point) 'rtf-p (current-buffer) (point-max))))
	   (let ((,res (progn ,@body)))
	     (cond ((< (point) ,min) (goto-char ,min) nil)
		   ((> (point) ,max) (goto-char ,max) nil)
		   (t ,res)))))))

(defun sclang-help-mode-beginning-of-defun (&optional arg)
  (interactive "p")
  (sclang-help-mode-limit-point-to-code (sclang-beginning-of-defun arg)))

(defun sclang-help-mode-end-of-defun (&optional arg)
  (interactive "p")
  (sclang-help-mode-limit-point-to-code (sclang-end-of-defun arg)))

(defun sclang-help-mode-fontify-region (start end loudly)
  (flet ((fontify-code
	  (start end loudly)
	  (funcall 'font-lock-default-fontify-region start end loudly))
	 (fontify-non-code
	  (start end loudly)
	  (while (< start end)
	    (let ((value (plist-get (text-properties-at start) 'rtf-face))
		  (end (next-single-property-change start 'rtf-face (current-buffer) end)))
		(add-text-properties start end (list 'face (append '(variable-pitch) (list value))))
		(setq start end)))))
    (let ((modified (buffer-modified-p)) (buffer-undo-list t)
	  (inhibit-read-only t) (inhibit-point-motion-hooks t)
	  (inhibit-modification-hooks t)
	  deactivate-mark buffer-file-name buffer-file-truename
	  (pos start))
      (unwind-protect
	  (while (< pos end)
	    (let ((end (next-single-property-change pos 'rtf-p (current-buffer) end)))
	      (if (sclang-rtf-p pos)
		  (fontify-non-code pos end loudly)
		(fontify-code pos end loudly))
	      (setq pos end)))
	(when (and (not modified) (buffer-modified-p))
	  (set-buffer-modified-p nil))))))


(defun sclang-help-mode-indent-line ()
  (if (sclang-code-p (point))
      (sclang-indent-line)
    (insert "\t")))

(define-derived-mode sclang-help-mode sclang-mode "SCLangHelp"
  "Major mode for displaying SuperCollider help files.
\\{sclang-help-mode-map}"
  (let ((file (or (buffer-file-name)
		  (and (boundp 'sclang-current-help-file)
		       sclang-current-help-file))))
    (when file
      (set-visited-file-name nil)
      (setq buffer-auto-save-file-name nil)
      (save-excursion
	(when (sclang-rtf-file-p file)
	  (let ((tmp-buffer (generate-new-buffer " *RTF*"))
		(modified-p (buffer-modified-p)))
	    (unwind-protect
		(progn
		  (sclang-convert-rtf-buffer tmp-buffer)
		  (toggle-read-only 0)
		  (erase-buffer)
		  (insert-buffer-substring tmp-buffer))
	      (and (buffer-modified-p) (not modified-p) (set-buffer-modified-p nil))
	      (kill-buffer tmp-buffer))))))
    (set (make-local-variable 'sclang-help-file) file)
    (setq font-lock-defaults
	  (append font-lock-defaults
		  '((font-lock-fontify-region-function . sclang-help-mode-fontify-region))))
    (set (make-local-variable 'beginning-of-defun-function) 'sclang-help-mode-beginning-of-defun)
    (set (make-local-variable 'indent-line-function) 'sclang-help-mode-indent-line)
    ))

;; =====================================================================
;; help file access
;; =====================================================================

(defun sclang-skip-help-directory-p (path)
  "Answer t if PATH should be skipped during help file indexing."
  (let ((directory (file-name-nondirectory path)))
    (reduce (lambda (a b) (or a b))
	    (mapcar (lambda (regexp) (string-match regexp directory))
		    '("^\.$" "^\.\.$" "^CVS$" "^\.svn$" "^_darcs$")))))

(defun sclang-filter-help-directories (list)
  "Remove paths to be skipped from LIST of directories."
  (remove-if (lambda (x)
	       (or (not (file-directory-p x))
		   (sclang-skip-help-directory-p x)))
	     list))

(defun sclang-directory-files-save (directory &optional full match nosort)
  "Return a list of names of files in DIRECTORY, or nil on error."
  (condition-case nil
      (directory-files directory full match nosort)
    (error nil)))

;; (defun sclang-extension-help-directories ()
;;   "Build a list of help directories for extensions."
;;   (flet ((flatten (seq)
;; 		  (if (null seq)
;; 		      seq
;; 		    (if (listp seq)
;; 			(reduce 'append (mapcar #'flatten seq))
;; 		      (list seq)))))
;;     (flatten
;;      (mapcar
;;       (lambda (dir)
;; 	(mapcar
;; 	 (lambda (dir)
;; 	   (remove-if-not
;; 	    'file-directory-p
;; 	    (sclang-directory-files-save dir t "^[Hh][Ee][Ll][Pp]$" t)))
;; 	 (sclang-filter-help-directories (sclang-directory-files-save dir t))))
;;       sclang-extension-path))))

;; (defun sclang-help-directories ()
;;   "Answer list of help directories to be indexed."
;;   (append sclang-help-path (sclang-extension-help-directories)))

(defun sclang-help-directories ()
  "Answer list of help directories to be indexed."
  (append sclang-help-path sclang-extension-path))

(defun sclang-make-help-topic-alist (dirs result)
  "Build a help topic alist from directories in DIRS, with initial RESULT."
  (if dirs
      (let* ((files (sclang-directory-files-save (car dirs) t))
	     (topics (remove-if 'null (mapcar 'sclang-help-topic-name files)))
	     (new-dirs	(sclang-filter-help-directories files)))
	(sclang-make-help-topic-alist
	 (append new-dirs (cdr dirs))
	 (append topics result)))
    (sort result (lambda (a b) (string< (car a) (car b))))))

(defun sclang-index-help-topics ()
  "Build an index of help topics searching in the various help file locations."
  (interactive)
  (setq sclang-help-topic-alist nil)
  (let ((case-fold-search nil)
	(max-specpdl-size 10000)
	(max-lisp-eval-depth 10000))
    (sclang-message "Indexing help topics ...")
    (setq sclang-help-topic-alist
	  (sclang-make-help-topic-alist (sclang-help-directories) nil))
    (sclang-message "Indexing help topics ... Done")))

(defun sclang-edit-html-help-file ()
  "Edit the help file associated with the current buffer.
Switches w3m to edit mode (actually HTML mode)."
  (interactive)
  (w3m-edit-current-url)
  )

(defun sclang-edit-help-code ()
  "Edit the help file to make code variations.
Switches to text mode with sclang-minor-mode."
  (interactive)
  (w3m-copy-buffer)
;;  (text-mode)
  (sclang-mode)
  (toggle-read-only)
  (rename-buffer "*SC_Help:CodeEdit*")
  )


(defun sclang-edit-help-file ()
  "Edit the help file associated with the current buffer.
Either visit file internally (.sc) or start external editor (.rtf)."
  (interactive)
  (if (and (boundp 'sclang-help-file) sclang-help-file)
      (let ((file sclang-help-file))
	(if (file-exists-p file)
	    (if (sclang-rtf-file-p file)
		(start-process (sclang-make-buffer-name (format "HelpEditor:%s" file))
			       nil sclang-rtf-editor-program file)
	      (find-file file))
	  (if (sclang-html-file-p file)
	      (w3m-edit-current-url)
	   ;; (find-file file)
	   )
	  (sclang-message "Help file not found")))
    (sclang-message "Buffer has no associated help file")))

(defun sclang-help-topic-at-point ()
  "Answer the help topic at point, or nil if not found."
  (save-excursion
    (with-syntax-table sclang-help-mode-syntax-table
      (let (beg end)
	(skip-syntax-backward "w_")
	(setq beg (point))
	(skip-syntax-forward "w_")
	(setq end (point))
	(goto-char beg)
	(car (assoc (buffer-substring-no-properties beg end)
		    sclang-help-topic-alist))))))

(defun sclang-goto-help-browser ()
  "Switch to the *w3m* buffer to browse help files"
  (interactive)
  (let* ((buffer-name "*w3m*")
	 (buffer (get-buffer buffer-name)))
    (if buffer
      (switch-to-buffer buffer)
      ;; else
      (let* ((buffer-name "*SC_Help:w3m*")
	     (buffer2 (get-buffer buffer-name)))
	(if buffer2
	    (switch-to-buffer buffer2)
	  ;; else
	  (sclang-find-help "Help")
	  )
	)
      )
    (if buffer
	(with-current-buffer buffer
	  (rename-buffer "*SC_Help:w3m*")
	  (sclang-help-minor-mode)
	  ;;(setq buffer-read-only false)
	  )
      )
;    (if buffer
;
;      )
    )
  )

(defun sclang-find-help (topic)
  (interactive
   (list
    (let ((topic (or (and mark-active (buffer-substring-no-properties (region-beginning) (region-end)))
                     (sclang-help-topic-at-point)
                     "Help")))
      (completing-read (format "Help topic%s: " (if (sclang-get-help-file topic)
                                                    (format " (default %s)" topic) ""))
                       sclang-help-topic-alist nil t nil 'sclang-help-topic-history topic))))
  (let ((file (sclang-get-help-file topic)))
    (if file
        (if (file-exists-p file)
            (let* ((buffer-name (sclang-help-buffer-name topic))
                   (buffer (get-buffer buffer-name)))
              (unless buffer
                (if (sclang-html-file-p file)
                    (w3m-find-file file)
                  ;;  (sclang-goto-help-browser)
                  ;; not a sclang-html file
                  (setq buffer (get-buffer-create buffer-name))
                  (with-current-buffer buffer
                    (insert-file-contents file)
                    (let ((sclang-current-help-file file)
                          (default-directory (file-name-directory file)))
                      (sclang-help-mode))
                    (set-buffer-modified-p nil)))
                (switch-to-buffer buffer))
              (if (sclang-html-file-p file)
                  (sclang-goto-help-browser))
              )
          (sclang-message "Help file not found") nil)
      (sclang-message "No help for \"%s\"" topic) nil)))


(defun sclang-open-help-gui ()
  "Open SCDoc Help Browser"
  (interactive)
  (sclang-eval-string (sclang-format "Help.gui"))
  )

(defvar sclang-scdoc-topics (make-hash-table :size 16385)
  "List of all scdoc topics.")

(sclang-set-command-handler
 'helpSymbols
 (lambda (list-of-symbols)
   (mapcar (lambda (arg)
             (puthash arg nil sclang-scdoc-topics))
           list-of-symbols)
   ))

(defun sclang-find-help-in-gui (topic)
  "Search for topic in SCDoc Help Browser"
  (interactive
   (list
    (let ((topic (sclang-symbol-at-point)))
      (completing-read (format "Help topic%s: " (if topic
                                                    (format " (default %s)" topic)
                                                  ""))
                       sclang-scdoc-topics nil nil nil 'sclang-help-topic-history topic)))
   )
  (if topic
      (sclang-eval-string (sclang-format "HelpBrowser.openHelpFor(%o)" topic))
    (sclang-eval-string (sclang-format "Help.gui"))
    )
  )


;; =====================================================================
;; module setup
;; =====================================================================

(add-hook 'sclang-library-startup-hook
          (lambda ()
            (sclang-perform-command 'helpSymbols)
            (condition-case nil
                (sclang-index-help-topics)
              (error nil))))

(add-hook 'sclang-library-shutdown-hook
          (lambda ()
            (clrhash sclang-scdoc-topics)))

(add-to-list 'auto-mode-alist '("\\.rtf$" . sclang-help-mode))
;; ========= ADDITION for HTML help files?? ============
;; (add-to-list 'auto-mode-alist '("\\.html$" . sclang-help-mode))
;; (setq mm-text-html-renderer 'w3m)
;;  (setq mm-inline-text-html-with-images t)
;;  (setq mm-inline-text-html-with-w3m-keymap nil)
;; =====================================================
(sclang-fill-help-syntax-table sclang-help-mode-syntax-table)
(sclang-fill-help-mode-map sclang-help-mode-map)

(provide 'sclang-help)

;; EOF
