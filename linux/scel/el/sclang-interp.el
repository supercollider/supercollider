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

;; =====================================================================
;; post buffer access
;; =====================================================================

(defconst sclang-post-buffer "*SCLang*"
  "Name of the SuperCollider process output buffer.")

(defvar sclang-post-buffer-mode-string nil
  "Mode string used in process buffer.")

(defvar sclang-post-buffer-mode-line-format '("-"
					      mode-line-mule-info
					      mode-line-frame-identification
					      mode-line-buffer-identification
					      "  "
					      sclang-post-buffer-mode-string)
  "*Modeline format for SuperCollider process buffer.")

(defun sclang-get-post-buffer ()
  (get-buffer sclang-post-buffer))

(defmacro with-sclang-post-buffer (&rest body)
  `(with-current-buffer (sclang-get-post-buffer)
     ,@body))

(defun sclang-post-string (string)
  (with-sclang-post-buffer
   (let ((eobp (mapcar (lambda (w)
			 (cons w (= (window-point w) (point-max))))
		       (get-buffer-window-list (current-buffer) nil t))))
     (save-excursion
       ;; insert STRING into process buffer
       (goto-char (point-max))
       (insert string))
     (dolist (assoc eobp)
       (when (cdr assoc)
	 (save-selected-window
	   (let ((window (car assoc)))
	     (select-window window)
	     (set-window-point window (point-max))
	     (recenter -1))))))))

(defun sclang-show-post-buffer (&optional eob-p)
  "Show SuperCollider process buffer.
With argument, positions cursor at end of buffer."
  (interactive "P")
  (with-sclang-post-buffer
   (let ((window (display-buffer (current-buffer))))
     (when eob-p
       (save-selected-window
	 (set-window-point window (point-max)))))))

(defun sclang-clear-post-buffer ()
  "Clear the output buffer."
  (interactive)
  (with-sclang-post-buffer (erase-buffer)))

(defun sclang--init-post-buffer ()
  (get-buffer-create sclang-post-buffer)
  (with-sclang-post-buffer
   (sclang-mode)
   (setq font-lock-defaults nil)
   (setq mode-line-format sclang-post-buffer-mode-line-format))
  (sclang-clear-post-buffer)
  (sclang-show-post-buffer))

;; =====================================================================
;; interpreter interface
;; =====================================================================

(defconst sclang-process "SCLang"
  "Name of the SuperCollider interpreter subprocess.")

(defcustom sclang-program "sclang"
  "*Name of the SuperCollider interpreter program."
  :group 'sclang-interface
  :type 'string)

(defcustom sclang-runtime-directory nil
  "*Path to the SuperCollider runtime directory."
  :group 'sclang-interface
  :version "21.3"
  :type 'directory
  :options '(:must-match))

;; =====================================================================
;; helper functions
;; =====================================================================

(defun sclang-get-process ()
  (get-process sclang-process))

;; =====================================================================
;; library startup/shutdown
;; =====================================================================

(defvar sclang--library-initialized-p nil)

(defcustom sclang-library-startup-hook nil
  "*Hook run after initialization of the SCLang process."
  :group 'sclang-interface
  :type 'hook)

(defcustom sclang-library-shutdown-hook nil
  "*Hook run before deletion of the SCLang process."
  :group 'sclang-interface
  :type 'hook)

;; library initialization works like this:
;;
;; sclang--init-command-process sets up sclang command interface
;; (called when 'Class tree inited' appears in sclang output)
;;
;; sclang sends '_init' command reply
;;
;; '_init' command handler calls sclang--on-library-startup to complete
;; initialization

(defun sclang-library-initialized-p ()
  (and (sclang-get-process)
       sclang--library-initialized-p))

(defun sclang--on-library-startup ()
  (setq sclang--library-initialized-p t)
  (run-hooks 'sclang-library-startup-hook))

(defun sclang--on-library-shutdown ()
  (run-hooks 'sclang-library-shutdown-hook)
  (setq sclang--library-initialized-p nil))

;; =====================================================================
;; process hooks
;; =====================================================================

(defun sclang-process-sentinel (proc msg)
  (sclang-post-string (format "%s %s" proc msg))
  (when (memq (process-status proc) '(exit signal))
    (sclang--on-library-shutdown)
    (sclang--stop-command-process)))

(defun sclang-process-filter (proc string)
  (when (and (not (sclang-library-initialized-p))
	     (string-match "Class tree inited" string))
    (sclang--init-command-process))
  (sclang-post-string string))

;; =====================================================================
;; process startup/shutdown
;; =====================================================================

(defun sclang-start ()
  "Start SuperCollider process."
  (interactive)
  (sclang-stop)
  (sclang--init-post-buffer)
  (sclang--start-command-process)
  (let ((process-connection-type nil))
    (let ((proc (start-process sclang-process nil
			       sclang-program
			       "-d" (expand-file-name (or sclang-runtime-directory
							  default-directory)))))
      (set-process-sentinel proc 'sclang-process-sentinel)
      (set-process-filter proc 'sclang-process-filter)
      (process-kill-without-query proc)
      proc)))

(defun sclang-stop ()
  "Stop SuperCollider process."
  (interactive)
  (when (sclang-get-process)
    (delete-process (sclang-get-process))))

;; =====================================================================
;; code evaluation
;; =====================================================================

(defcustom sclang-eval-line-forward nil
  "*If non-nil `sclang-eval-line' advances to the next line."
  :group 'sclang-interface
  :type 'boolean)

(defun sclang-send-string (string &optional print-p)
  "Send STRING to the sclang process for execution and print the result
if PRINT-P is non-nil. Return STRING if successful, otherwise nil."
  (let ((proc (sclang-get-process)))
    (when proc
      (process-send-string proc (concat string (if print-p "\^L" "\^[")))
      string)))

(defun sclang-eval-string (string &optional silent-p)
  (interactive "sEnter string: \nP")
  (sclang-send-string string (not silent-p)))

(defun sclang-eval-line (&optional silent-p)
  (interactive "P")
  (let ((string (sclang-line-at-point)))
    (when string
      (sclang-send-string string (not silent-p))
      (and sclang-eval-line-forward
	   (/= (line-end-position) (point-max))
	   (next-line 1))
      string)))

(defun sclang-eval-region (&optional silent-p)
  (interactive "P")
  (if mark-active
      (sclang-send-string (buffer-substring-no-properties (region-beginning)
							  (region-end))
			  (not silent-p))
    (sclang-eval-line silent-p)))

(defun sclang-eval-defun (&optional silent-p)
  (interactive "P")
  (let ((string (sclang-defun-at-point)))
    (when (and string (string-match "^(" string))
      (sclang-send-string string (not silent-p))
      string)))

;; =====================================================================
;; command process
;; =====================================================================

(defcustom sclang-mkfifo-program "mkfifo"
  "*Name of the \"mkfifo\" program.

Change this if \"mkfifo\" has a non-standard name or location."
  :group 'sclang-interface
  :type 'string)

(defcustom sclang-cat-program "cat"
  "*Name of the \"cat\" program.

Change this if \"cat\" has a non-standard name or location."
  :group 'sclang-interface
  :type 'string)

(defconst sclang--command-process "SCLang Command")
(defconst sclang--command-buffer (concat " *" sclang--command-process "*"))

(defvar sclang--command-fifo nil
  "FIFO for communicating with the subprocess.")

;; (defvar sclang--command-default-file "sclang-default-commands.sc")

(defun sclang--create-command-fifo ()
  (unless sclang--command-fifo
    (setq sclang--command-fifo (make-temp-name
				(expand-file-name
				 (concat temporary-file-directory
					 "sclang-command-fifo."))))
    (unless (file-exists-p sclang--command-fifo)
      (unless (= 0 (call-process sclang-mkfifo-program
				 nil nil nil
				 sclang--command-fifo))
	(setq sclang--command-fifo nil)))))

(defun sclang--command-process-sentinel (proc msg)
  (sclang-post-string (format "%s %s" proc msg))
  (when (and (memq (process-status proc) '(exit signal))
	     sclang--command-fifo
	     (file-exists-p sclang--command-fifo))
    (delete-file sclang--command-fifo)
    (setq sclang--command-fifo nil)))

(defun sclang--command-process-filter (proc string)
  (with-current-buffer sclang--command-buffer
    ;; append STRING to command buffer
    (goto-char (point-max))
    (insert string)
    ;; try reading expressions
    (while (progn
	     (goto-char (point-min))
	     (condition-case nil
		 (let ((assoc (read (current-buffer)))
		       (end (point)))
		   (save-excursion (sclang--handle-command-result assoc))
		   ;; delete expression read
		   (delete-region (point-min) end)
		   t)
	       (error nil))))))

(defun sclang--start-command-process ()
  (sclang--create-command-fifo)
  (when (and sclang--command-fifo
	     (not (get-process sclang--command-process)))
    (let ((process-connection-type nil))
      (let ((proc (start-process sclang--command-process
				 sclang--command-buffer
				 sclang-cat-program sclang--command-fifo)))
	(set-process-sentinel proc 'sclang--command-process-sentinel)
	(set-process-filter proc 'sclang--command-process-filter)
	(process-kill-without-query proc)))))

(defun sclang--stop-command-process ()
  (when (get-process sclang--command-process)
    (delete-process sclang--command-process)))

;; (defun sclang-first-file-in-path (file-name path)
;;   (let (result)
;;     (while (and path (null result))
;;       (let ((file (expand-file-name
;; 		   (concat (file-name-as-directory (car path))
;; 			   file-name))))
;; 	(if (file-exists-p file)
;; 	    (setq result file))))
;;     result))

;; (defun sclang--init-command-process ()
;;   (let ((fifo sclang--command-fifo)
;; 	(file (sclang-first-file-in-path sclang--command-default-file
;; 					 load-path)))
;;     (and fifo file
;; 	 (sclang-send-string
;; 	  (sclang-format "Emacs.lispInit(%o, %o)" fifo file)))))

(defun sclang--init-command-process ()
  (when sclang--command-fifo
    (sclang-send-string
     (sclang-format "Emacs.lispInit(%o)" sclang--command-fifo))))

;; =====================================================================
;; command interface
;; =====================================================================

;; symbol property: sclang-command-handler

(defun sclang-set-command-handler (symbol function)
  ;; (add-to-list 'sclang--command-handler-alist (cons name function)))
  (put symbol 'sclang-command-handler function))

(defun sclang-perform-command (symbol &rest args)
  (and (functionp (get symbol 'sclang-command-handler))
       (not (eq ?_ (aref (symbol-name symbol) 0)))
       (sclang-send-string (sclang-format "Emacs.lispPerformCommand(%o, %o)"
					  symbol args))))

(defun sclang--handle-command-result (assoc)
  (condition-case e
      (let ((fun (get (car assoc) 'sclang-command-handler)))
	(when (functionp fun)
	  (funcall fun (cdr assoc))
	  t))
    (error
     (message "sclang--handle-command-result: %s"
	      (error-message-string e))
     nil)))

;; =====================================================================
;; default command handlers
;; =====================================================================

(sclang-set-command-handler
 '_init
 (lambda (arg)
   (sclang--on-library-startup)))

(sclang-set-command-handler
 '_eval
 (lambda (data)
   (message "%s" data)
   (let ((id (nth 0 data))
	 (func (intern-soft (nth 1 data)))
	 (args (nth 2 data)))
     (when (fboundp func)
       (let ((res (apply func args)))
	 (sclang-send-string
	  (sclang-format "Emacs.lispHandleCommandResult(%o, %o)" id res)))))))

;; =====================================================================
;; module setup
;; =====================================================================

;; shutdown process cleanly
(add-hook 'kill-emacs-hook (lambda () (sclang-stop)))

;; add command line switch
(add-to-list 'command-switch-alist
	     (cons "sclang" (lambda (switch) (sclang-start))))

(provide 'sclang-interp)

;; EOF