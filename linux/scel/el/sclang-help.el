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

(defcustom sclang-help-directory "~/SuperCollider/Help"
  "*Directory where the SuperCollider help files are kept."
  :group 'sclang-interface
  :version "21.3"
  :type 'directory
  :options '(:must-match))

(defvar sclang-help-topic-alist nil)
(defvar sclang-help-topic-regexp nil)

(defun sclang-index-help-topics ()
  (interactive)
  (if sclang-help-directory
      (let ((case-fold-search nil)
	    (max-specpdl-size 10000)
	    result)
	(flet ((index (dir)
		      (dolist (file (directory-files dir t "^[^.]" t))
			(cond ((file-directory-p file)
			       ;; recurse into sub-directories
			       (unless (string-match "CVS$" file) (index file)))
			      ((string-match "\\(\\(\\.help\\)?\\.\\(rtf\\|sc\\)\\)$" file)
			       (push (cons (file-name-nondirectory (replace-match "" nil nil file 1)) file)
				     result))))))
	  (message "Indexing help topics ...")
	  (index sclang-help-directory)
	  (let ((list (sort result (lambda (a b) (string< (car a) (car b))))))
	    (setq sclang-help-topic-alist list
		  sclang-help-topic-regexp (concat "\\(\\s \\|\\s.\\)+"
						   (regexp-opt (mapcar 'car list) t)
						   "\\(\\s \\|\\s.\\)+")))
	  (message "Indexing help topics ... Done")))
    (setq sclang-help-topic-alist nil)
    (message "Help directory is unset")))

(defun sclang-help-file (topic)
  (cdr (assoc topic sclang-help-topic-alist)))

(defun sclang-un-rtfify-buffer (&optional buffer)
  (with-current-buffer (or buffer (current-buffer))
    (let ((doc-regexp "{\\\\rtf")
	  (table-regexp (concat "{\\\\\\("
				(regexp-opt '("colortbl" "filetbl" "fonttbl" "stylesheet")) 
				"\\)"))
	  (control-regexp "\\\\\\(\\([{}\\\t\n]\\)\\|\\(\\(par\\|tab\\)?[^\\ \n]+ ?\\)\\)")
	  (case-fold-search nil))
      (save-excursion
	(goto-char (point-min))
	(when (looking-at doc-regexp)
	  (while (re-search-forward "[{}\\\n]" nil t)
	    (goto-char (match-beginning 0))
	    (cond ((looking-at table-regexp)
		   (let ((beg (point)))
		     (forward-list 1)
		     (delete-region beg (point))))
		  ((looking-at control-regexp)
		   (replace-match
		    (or (match-string 2)	;; escape
			(let ((control (match-string 4)))
			  (cond ((string= control "par") "\n")
				((string= control "tab") "\t")
				(t ""))))
		    'fixedcase 'literal))
		  (t (delete-char 1)))))))))

(defun sclang-compress-newlines (&optional buffer)
  (with-current-buffer (or buffer (current-buffer))
    (save-excursion
      (goto-char (point-min))
      (while (not (eobp))
	(if (and (bolp) (eolp)
		 (save-excursion
		   (forward-line -1)
		   (and (bolp) (eolp))))
	    (delete-char 1)
	  (forward-line 1))))))

(defun sclang-linkify-help-buffer (&optional buffer)
  (with-current-buffer (or buffer (current-buffer))
    (let ((case-fold-search nil))
      (save-excursion
	(goto-char (point-min))
	(while (re-search-forward sclang-help-topic-regexp nil t)
	  (let ((topic (match-string 2)))
	    (replace-match
	     (propertize (sclang-browser-make-link topic topic)
			 'face 'bold)
	     'fixedcase 'literal nil 2)))))))

(defconst sclang-help-buffer "*SCLang Help*")
(defvar sclang-help-topic-history nil)
(defvar sclang-help-topic-ring-length 32)
(defvar sclang-help-topic-ring (make-ring sclang-help-topic-ring-length))

(defun sclang-fill-help-mode-map (map)
;; 	 (define-key sclang-browser-mode-map "\C-c\C-b" 'sclang-help-back)
  (define-key map "\C-\M-x" 'sclang-eval-defun)
  (define-key map "\C-c\C-x" 'sclang-eval-region))

(defun sclang--find-help-topic (topic)
  (let ((file (sclang-help-file topic))
	(special-display-buffer-names (list sclang-help-buffer)))
    (if (and file (file-exists-p file))
	(with-sclang-browser
	 sclang-help-buffer
	 (erase-buffer)
	 (insert-file-contents file)
	 (sclang-un-rtfify-buffer)
	 (sclang-compress-newlines)
	 (sclang-linkify-help-buffer)
	 (setq sclang-browser-link-function 'sclang-help-follow-link)
	 ;; FIXME: better factoring!
	 (let ((map (make-sparse-keymap)))
	   (set-keymap-parent map sclang-browser-mode-map)
	   (sclang-fill-help-mode-map map)
	   (use-local-map map))
	 (set-buffer-modified-p nil)
	 (current-buffer))
      (message "No help file for \"%s\"" topic) nil)))

(defun sclang-help-follow-link (topic)
  (interactive)
  (with-current-buffer sclang-help-buffer
    (ring-insert-at-beginning sclang-help-topic-ring
			      (cons topic (point))))
  (sclang--find-help-topic topic))

;; FIXME: doesn't work yet
(defun sclang-help-back ()
  (interactive)
  (let ((assoc (unless (ring-empty-p sclang-help-topic-ring)
		 (ring-remove sclang-help-topic-ring))))
    (when assoc
      (sclang--find-help-topic (car assoc))
      (with-current-buffer (get-buffer sclang-help-buffer)
	(goto-char (cdr assoc))))))

(defun sclang-find-help (topic)
  (interactive
   (list
    (let ((symbol (sclang-symbol-at-point)))
      (completing-read (format "Help topic%s: " (if (sclang-help-file symbol)
						   (format " (default %s)" symbol) ""))
		       sclang-help-topic-alist nil t nil sclang-help-topic-history symbol))))
  (when (sclang--find-help-topic topic)
    (setq sclang-help-topic-ring (make-ring sclang-help-topic-ring-length))))

(add-hook 'sclang-library-startup-hook 'sclang-index-help-topics)