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

;; (defvar sclang-post-buffer-mode-string nil
;;   "Mode string used in process buffer.")
;; 
;; (defvar sclang-post-buffer-mode-line-format '("-"
;; 					      mode-line-mule-info
;; 					      mode-line-frame-identification
;; 					      mode-line-buffer-identification
;; 					      "  "
;; 					      sclang-post-buffer-mode-string)
;;   "*Modeline format for SuperCollider process buffer.")

(defun sclang-get-post-buffer ()
  (get-buffer sclang-post-buffer))

(defmacro with-sclang-post-buffer (&rest body)
  `(with-current-buffer (sclang-get-post-buffer)
     ,@body))

;; (defun sclang-post-string (string)
;;   (with-sclang-post-buffer
;;    (let ((eobp (mapcar (lambda (w)
;; 			 (cons w (= (window-point w) (point-max))))
;; 		       (get-buffer-window-list (current-buffer) nil t))))
;;      (save-excursion
;;        ;; insert STRING into process buffer
;;        (goto-char (point-max))
;;        (insert string))
;;      (dolist (assoc eobp)
;;        (when (cdr assoc)
;; 	 (save-selected-window
;; 	   (let ((window (car assoc)))
;; 	     (select-window window)
;; 	     (set-window-point window (point-max))
;; 	     (recenter -1))))))))

(defun sclang-show-post-buffer (&optional eob-p)
  "Show SuperCollider process buffer.
If EOB-P is non-nil, positions cursor at end of buffer."
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
   (setq font-lock-defaults nil))
;;   (sclang-clear-post-buffer)
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
;; * emacs starts sclang with SCLANG_COMMAND_FIFO set in the environment
;; * sclang sends '_init' command reply during class tree initialization
;; * '_init' command handler calls sclang--on-library-startup to complete
;;   initialization

(defun sclang-library-initialized-p ()
  (and (sclang-get-process)
       sclang--library-initialized-p))

(defun sclang--on-library-startup ()
  (message "SCLang: Initializing library ...")
  (setq sclang--library-initialized-p t)
  (run-hooks 'sclang-library-startup-hook)
  (message "SCLang: Initializing library ... Done"))

(defun sclang--on-library-shutdown ()
  (run-hooks 'sclang-library-shutdown-hook)
  (setq sclang--library-initialized-p nil))

;; =====================================================================
;; process hooks
;; =====================================================================

(defun sclang--process-sentinel (proc msg)
  (message (format "%s %s" proc (substring msg 0 -1)))
  (when (memq (process-status proc) '(exit signal))
    (sclang--on-library-shutdown)))

(defun sclang--process-filter (proc string)
  (let* ((buffer (process-buffer proc))
	 (window (display-buffer buffer)))
    (with-current-buffer buffer
      (let ((moving (= (point) (process-mark proc))))
	(save-excursion
	  ;; Insert the text, advancing the process marker.
	  (goto-char (process-mark proc))
	  (insert string)
	  (set-marker (process-mark proc) (point)))
	(when moving
	  (goto-char (process-mark proc))
	  (set-window-point window (process-mark proc)))))))

;; =====================================================================
;; process startup/shutdown
;; =====================================================================

;; (defun sclang-setup-runtime-directory (dir)
;;   (let ((prompt-fmt "Directory \"%s\" doesn't exist; create? "))
;;     (unless (file-exists-p dir)
;;       (when (y-or-n-p (format prompt-fmt dir))
;; 	(make-directory dir)))
;;     (when (file-directory-p dir)
;;       (let ((synthdef-dir (expand-file-name "synthdefs" dir)))
;; 	(unless (file-exists-p dir)
;; 	  (when (y-or-n-p (format prompt-fmt synthdef-dir))
;; 	    (make-directory synthdef-dir)))))))

(defun sclang-start ()
  "Start SuperCollider process."
  (interactive)
  (sclang-stop)
  (sit-for 1)
  (sclang--init-post-buffer)
  (sclang--start-command-process)
  ;; setup runtime directory structure
;;   (and sclang-runtime-directory
;;        (sclang-setup-runtime-directory sclang-runtime-directory))
  (let ((process-connection-type nil))
    (let ((proc (start-process
		 sclang-process sclang-post-buffer
		 sclang-program
		 "-d" (expand-file-name (or sclang-runtime-directory
					    default-directory)))))
      (set-process-sentinel proc 'sclang--process-sentinel)
      (set-process-filter proc 'sclang--process-filter)
      (set-process-coding-system proc 'latin-1-unix 'latin-1-unix)
      (process-kill-without-query proc)
      proc)))

(defun sclang-stop ()
  "Stop SuperCollider process."
  (interactive)
  (when (sclang-get-process)
    (process-send-eof sclang-process)
    (let ((tries 4)
	  (i 0))
      (while (and (sclang-get-process)
		  (< i tries))
	(incf i)
	(sit-for 0.5))))
  (sclang-kill))

(defun sclang-kill ()
  "Kill SuperCollider process."
  (interactive)
  (when (sclang-get-process)
    (kill-process sclang-process)
    (delete-process sclang-process)))

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

(defconst sclang--command-process "SCLang Command"
  "Subprocess for receiving command results from sclang.")

(defvar sclang--command-fifo nil
  "FIFO for communicating with the subprocess.")

(defun sclang--create-command-fifo ()
  (setq sclang--command-fifo (make-temp-name
			      (expand-file-name
			       (concat temporary-file-directory
				       "sclang-command-fifo."))))
  (and (file-exists-p sclang--command-fifo) (delete-file sclang--command-fifo))
  (let ((res (call-process sclang-mkfifo-program
			   nil t t
			   sclang--command-fifo)))
    (unless (eq 0 res)
      (message "SCLang: Couldn't create command fifo")
      (setq sclang--command-fifo nil))))

;; (defun sclang--create-command-fifo ()
;;   (let ((file  "/tmp/sclang-command.fifo"))
;; ;;     (and (file-exists-p file) (delete-file file))
;; ;;     (call-process sclang-mkfifo-program nil nil nil file)
;;     (message (shell-command-to-string (format "rm -f %s; mkfifo %s" file file)))
;;     (setq sclang--command-fifo file)))

(defun sclang--command-process-sentinel (proc msg)
  (when (and (memq (process-status proc) '(exit signal))
	     sclang--command-fifo
	     (file-exists-p sclang--command-fifo))
    (delete-file sclang--command-fifo)
    (setq sclang--command-fifo nil)))

(defun sclang--start-command-process ()
  (sclang--create-command-fifo)
  (when sclang--command-fifo
    ;; sclang gets the fifo path via the environment
    (setenv "SCLANG_COMMAND_FIFO" sclang--command-fifo)
    (let ((process-connection-type nil))
      (let ((proc (start-process
		   sclang--command-process nil
		   sclang-cat-program sclang--command-fifo)))
	(set-process-sentinel proc 'sclang--command-process-sentinel)
	(set-process-filter proc 'sclang--command-process-filter)
	;; this is important. use a unibyte stream without eol
	;; conversion for communication.
	(set-process-coding-system proc 'no-conversion 'no-conversion)
	(process-kill-without-query proc)))
    (unless (get-process sclang--command-process)
      (message "SCLang: Couldn't start command process"))))

(defvar sclang--command-state [sclang--gather-command-output 0 ""]
  "State array for processing command process output.

[HANDLER EXPECTED-STRING-SIZE OUTPUT-STRING]")

(defun sclang--gather-command-output (state)
  (aset state 0 'sclang--gather-command-output)
  (let ((string (aref state 2)))
    (when (> (length string) 3)
    (aset state 1 (sclang-string-to-int32 string))
    (aset state 2 (substring string 4))
    (sclang--scatter-command-output state))))

(defun sclang--scatter-command-output (state)
  (aset state 0 'sclang--scatter-command-output)
  (let ((size (aref state 1))
	(string (aref state 2)))
  (when (and (> size 0) (>= (length string) size))
    (let ((assoc (car (read-from-string string 0 size))))
      (sclang--handle-command-result assoc))
    (aset state 1 0)
    (aset state 2 (substring string size))
    (sclang--gather-command-output state))))

(defun sclang--command-process-filter (proc string)
  (let ((state sclang--command-state))
    (aset state 2 (concat (aref state 2) string))
    (funcall (aref state 0) state)))

;; =====================================================================
;; command interface
;; =====================================================================

;; symbol property: sclang-command-handler

(defvar sclang-debug-command-handler nil)

(defun sclang-set-command-handler (symbol function)
  (put symbol 'sclang-command-handler function))

(defun sclang-perform-command (symbol &rest args)
;;   (and (functionp (get symbol 'sclang-command-handler))
;;        (not (eq ?_ (aref (symbol-name symbol) 0)))
  (sclang-eval-string (sclang-format
		       "Emacs.lispPerformCommand(%o, %o, true)"
		       symbol args)))

(defun sclang-perform-command-no-result (symbol &rest args)
;;   (and (functionp (get symbol 'sclang-command-handler))
;;        (not (eq ?_ (aref (symbol-name symbol) 0)))
  (sclang-eval-string (sclang-format
		       "Emacs.lispPerformCommand(%o, %o, false)"
		       symbol args)))

(defun sclang--handle-command-result (list)
  (save-excursion
    (condition-case nil
	(let ((fun (get (nth 0 list) 'sclang-command-handler))
	      (arg (nth 1 list))
	      (id  (nth 2 list)))
	  (when (functionp fun)
	    (let ((res (condition-case nil
			   (funcall fun arg)
			 (error (if sclang-debug-command-handler
				    (debug e)
				  (message "SCLang: Error in command handler") nil)))))
	      (when id
		(sclang-eval-string
		 (sclang-format "Emacs.lispHandleCommandResult(%o, %o)" id res))))))
      (error nil))))

;; =====================================================================
;; code evaluation
;; =====================================================================

(defconst sclang--token-interpret-cmd-line (char-to-string #X1b))
(defconst sclang--token-interpret-print-cmd-line (char-to-string #X0c))

(defcustom sclang-eval-line-forward t
  "*If non-nil `sclang-eval-line' advances to the next line."
  :group 'sclang-interface
  :type 'boolean)

(defun sclang-send-string (token string)
  (let ((proc (sclang-get-process)))
    (when proc
      (process-send-string proc (concat string token))
      string)))

(defun sclang-eval-string (string &optional print-p)
  "Send STRING to the sclang process for evaluation and print the result
if PRINT-P is non-nil. Return STRING if successful, otherwise nil."
  (sclang-send-string
   (if print-p sclang--token-interpret-print-cmd-line sclang--token-interpret-cmd-line)
   string))

(defun sclang-eval-expression (string &optional silent-p)
  (interactive "sEval: \nP")
  (sclang-eval-string string (not silent-p)))

(defun sclang-eval-line (&optional silent-p)
  (interactive "P")
  (let ((string (sclang-line-at-point)))
    (when string
      (sclang-eval-string string (not silent-p))
      (and sclang-eval-line-forward
	   (/= (line-end-position) (point-max))
	   (next-line 1))
      string)))

(defun sclang-eval-region (&optional silent-p)
  (interactive "P")
  (sclang-eval-string
   (buffer-substring-no-properties (region-beginning) (region-end))
   (not silent-p)))

(defun sclang-eval-region-or-line (&optional silent-p)
  (interactive "P")
  (if (and transient-mark-mode mark-active)
      (sclang-eval-region silent-p)
    (sclang-eval-line silent-p)))

(defun sclang-eval-defun (&optional silent-p)
  (interactive "P")
  (let ((string (sclang-defun-at-point)))
    (when (and string (string-match "^(" string))
      (sclang-eval-string string (not silent-p))
      string)))

;; =====================================================================
;; workspace
;; =====================================================================

(defcustom sclang-show-workspace-on-startup t
  "*If non-nil show the workspace buffer on library startup."
  :group 'sclang-interface
  :type 'boolean)

(defvar sclang-workspace-buffer "*SCWorkspace*")

(defun sclang-fill-workspace-mode-map (map)
  (define-key map "\C-c}" 'bury-buffer))

(defun sclang-switch-to-workspace ()
  (interactive)
  (let ((buffer (get-buffer sclang-workspace-buffer)))
    (unless buffer
      (setq buffer (get-buffer-create sclang-workspace-buffer))
      (with-current-buffer buffer
	(sclang-mode)
	(let ((map (make-sparse-keymap)))
	  (set-keymap-parent map sclang-mode-map)
	  (sclang-fill-workspace-mode-map map)
	  (use-local-map map))
	(let ((line (concat "// " (make-string 69 ?=) "\n")))
	  (insert line)
	  (insert "// SuperCollider Workspace\n")
	  (insert line)
	  (insert "\n"))
	(set-buffer-modified-p nil)))
    (switch-to-buffer buffer t)))

(add-hook 'sclang-library-startup-hook 
	  (lambda () (and sclang-show-workspace-on-startup
			  (sclang-switch-to-workspace))))

;; =====================================================================
;; language control
;; =====================================================================

(defun sclang-main-run ()
  (interactive)
  (sclang-eval-string "thisProcess.run"))

(defun sclang-main-stop ()
  (interactive)
  (sclang-eval-string "thisProcess.stop"))

;; =====================================================================
;; default command handlers
;; =====================================================================

(sclang-set-command-handler '_init (lambda (arg) (sclang--on-library-startup)))

(sclang-set-command-handler
 '_eval
 (lambda (expr)
   (when (stringp expr)
     (eval (read expr)))))

;; =====================================================================
;; module setup
;; =====================================================================

;; shutdown process cleanly
(add-hook 'kill-emacs-hook (lambda () (sclang-stop)))

;; add command line switches
(add-to-list 'command-switch-alist
	     (cons "sclang"
		   (lambda (switch)
		     (sclang-start))))

(add-to-list 'command-switch-alist
	     (cons "scmail"
		   (lambda (switch)
		     (sclang-start)
		     (when command-line-args-left
		       (let ((file (pop command-line-args-left)))
			 (with-current-buffer (get-buffer-create "*SCLang Workspace*")
			   (and (file-exists-p file) (insert-file-contents file))
			   (set-buffer-modified-p nil)
			   (sclang-mode)
			   (switch-to-buffer (current-buffer))))))))

(provide 'sclang-interp)

;; EOF