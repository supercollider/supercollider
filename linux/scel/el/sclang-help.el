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

(eval-when-compile (require 'cl))

(defcustom sclang-help-directory "~/SuperCollider/Help"
  "*Directory where the SuperCollider help files are kept."
  :group 'sclang-interface
  :version "21.3"
  :type 'directory
  :options '(:must-match))

(defcustom sclang-rtf-editor-program "ted"
  "*Name of an RTF editor program used to edit SuperCollider help files."
  :group 'sclang-interface
  :type 'string)

(defvar sclang-help-syntax-table nil
  "Syntax table used in SuperCollider help buffers.")

(defvar sclang-help-topic-alist nil)
(defvar sclang-help-topic-history nil)
(defvar sclang-help-topic-ring-length 32)
(defvar sclang-help-topic-ring (make-ring sclang-help-topic-ring-length))

(defun sclang-get-help-file (topic)
  (cdr (assoc topic sclang-help-topic-alist)))

(defun sclang--help-buffer-name (topic)
  (concat "*SCHelp: " topic "*"))

(defun sclang--rtf-file-p (file-name)
  (let ((case-fold-search t))
    (string-match ".*\\.rtf$" file)))

(defun sclang--sc-file-p (file-name)
  (let ((case-fold-search t))
    (string-match ".*\\.sc$" file)))
  
(defun sclang-fill-help-syntax-table (table)
  ;; make ?- be part of symbols for selection and sclang-symbol-at-point
  (modify-syntax-entry ?- "_" table))

(defun sclang-get-help-syntax-table ()
  (unless sclang-help-syntax-table
    (let ((table (make-syntax-table sclang-mode-syntax-table)))
      (sclang-fill-help-syntax-table table)
      (setq sclang-help-syntax-table table)))
  sclang-help-syntax-table)

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
	  (sclang-message "Indexing help topics ...")
	  (index sclang-help-directory)
	  (setq sclang-help-topic-alist (sort result (lambda (a b) (string< (car a) (car b)))))
	  (sclang-message "Indexing help topics ... Done")))
    (setq sclang-help-topic-alist nil)
    (sclang-message "Help directory is unset")))

(defun sclang-edit-help-file ()
  (interactive)
  (if (and (boundp 'sclang-help-file) sclang-help-file)
      (let ((file sclang-help-file))
	(if (file-exists-p file)
	    (if (sclang--rtf-file-p file)
		(start-process (format "*SCLang Help Editor %s*" file) nil sclang-rtf-editor-program file)
	      (find-file file))
	  (sclang-message "Help file not found")))
    (sclang-message "Buffer has no associated help file")))

(defun sclang-fill-help-mode-map (map)
  (define-key map "\C-c}" 'bury-buffer)
  (define-key map "\C-c\C-v" 'sclang-edit-help-file))

(defun sclang-help-topic-at-point ()
  "Answer the help topic at point, or nil if not found."
  (save-excursion
    (with-syntax-table (sclang-get-help-syntax-table)
      (let (beg end)
	(skip-syntax-backward "w_")
	(setq beg (point))
	(skip-syntax-forward "w_")
	(setq end (point))
	(goto-char beg)
	(buffer-substring-no-properties beg end)))))

(defun sclang-init-help-mode (file)
  (sclang-mode)
  (set (make-local-variable 'sclang-help-file) file)
  (set-syntax-table (sclang-get-help-syntax-table))
  (let ((map (make-sparse-keymap)))
    (set-keymap-parent map sclang-mode-map)
    (sclang-fill-help-mode-map map)
    (use-local-map map))
  (unless (sclang--sc-file-p file)
    ;; FIXME: (font-lock-mode -1) doesn't work
    (setq font-lock-defaults nil)
    (setq indent-line-function #'indent-to-left-margin)))

(defun sclang-display-help-file (topic file)
  (if (file-exists-p file)
      (let* ((buffer-name (sclang--help-buffer-name topic))
	     (buffer (get-buffer buffer-name)))
	(unless buffer
	  (setq buffer (get-buffer-create buffer-name))
	  (with-current-buffer buffer
	    (insert-file-contents file)
	    (when (sclang--rtf-file-p file)
	      (sclang-un-rtfify-buffer)
	      (sclang-compress-newlines))
	    (set-buffer-modified-p nil)
	    (goto-char (point-min))
	    (sclang-init-help-mode file)))
	(switch-to-buffer buffer t))
    (sclang-message "Help file not found")))

(defun sclang-find-help (topic)
  (interactive
   (list
    (let ((topic (or (and mark-active (buffer-substring-no-properties (region-beginning) (region-end)))
		     (sclang-help-topic-at-point))))
      (completing-read (format "Help topic%s: " (if (sclang-get-help-file topic)
						    (format " (default %s)" topic) ""))
		       sclang-help-topic-alist nil t nil sclang-help-topic-history topic))))
  (let ((file (sclang-get-help-file topic)))
    (if file
	(sclang-display-help-file topic file)
      (sclang-message "No help for \"%s\"" topic))))

;; =====================================================================
;; module setup
;; =====================================================================

(add-hook 'sclang-library-startup-hook 'sclang-index-help-topics)

;; EOF