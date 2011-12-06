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
  (require 'sclang-util)
  (require 'compile)
  )

;; =====================================================================
;; post buffer access
;; =====================================================================

;; FIXME: everything will fail when renaming the post buffer!

(defconst sclang-post-buffer (sclang-make-buffer-name "PostBuffer")
  "Name of the SuperCollider process output buffer.")

(defconst sclang-bullet-latin-1 (string-to-char (decode-coding-string "\xa5" 'utf-8))
  "Character for highlighting errors (latin-1).")

(defconst sclang-bullet-utf-8 (string-to-char (decode-coding-string "\xe2\x80\xa2" 'utf-8))
  "Character for highlighting errors (utf-8).")

(defconst sclang-parse-error-regexp
  "^\\(WARNING\\|ERROR\\): .*\n[\t ]*in file '\\([^']\+\\)'\n[\t ]*line \\([0-9]\+\\) char \\([0-9]\+\\)"
  "Regular expression matching parse errors during library compilation.")

(defcustom sclang-max-post-buffer-size 0
  "*Maximum number of characters to insert in post buffer.
Zero means no limit."
  :group 'sclang-interface
  :version "21.3"
  :type 'integer)

(defcustom sclang-auto-scroll-post-buffer nil
  "*Automatically scroll post buffer on output regardless of point position.
Default behavior is to only scroll when point is not at end of buffer."
  :group 'sclang-interface
  :version "21.3"
  :type 'boolean)

(defun sclang-get-post-buffer ()
  (get-buffer-create sclang-post-buffer))

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

;; (defun sclang-post-string (string &optional proc)
;;   (let* ((buffer (process-buffer proc))
;; 	 (window (display-buffer buffer)))
;;     (with-current-buffer buffer
;;       (let ((moving (= (point) (process-mark proc))))
;; 	(save-excursion
;; 	  ;; Insert the text, advancing the process marker.
;; 	  (goto-char (process-mark proc))
;; 	  (insert string)
;; 	  (set-marker (process-mark proc) (point)))
;; 	(when moving
;; 	  (goto-char (process-mark proc))
;; 	  (set-window-point window (process-mark proc)))))))

(defun sclang-show-post-buffer (&optional eob-p)
  "Show SuperCollider process buffer.
If EOB-P is non-nil, positions cursor at end of buffer."
  (interactive "P")
  (with-sclang-post-buffer
   (let ((window (display-buffer (current-buffer))))
     (when eob-p
       (goto-char (point-max))
       (save-selected-window
	 (set-window-point window (point-max)))))))

(defun sclang-clear-post-buffer ()
  "Clear the output buffer."
  (interactive)
  (with-sclang-post-buffer (erase-buffer)))

(defun sclang-init-post-buffer ()
  "Initialize post buffer."
  (get-buffer-create sclang-post-buffer)
  (with-sclang-post-buffer
   ;; setup sclang mode
   (sclang-mode)
   (set (make-local-variable 'font-lock-fontify-region-function)
	(lambda (&rest args)))
   ;; setup compilation mode
   (compilation-minor-mode)
   (set (make-variable-buffer-local 'compilation-error-screen-columns) nil)
   (set (make-variable-buffer-local 'compilation-error-regexp-alist)
	(cons (list sclang-parse-error-regexp 2 3 4) compilation-error-regexp-alist))
   (set (make-variable-buffer-local 'compilation-parse-errors-function)
	(lambda (limit-search find-at-least)
	  (compilation-parse-errors limit-search find-at-least)))
   (set (make-variable-buffer-local 'compilation-parse-errors-filename-function)
	(lambda (file-name)
	  file-name)))
  (sclang-clear-post-buffer)
  (sclang-show-post-buffer))

;; =====================================================================
;; interpreter interface
;; =====================================================================

(defconst sclang-process "SCLang"
  "Name of the SuperCollider interpreter subprocess.")

(defcustom sclang-program "sclang"
  "*Name of the SuperCollider interpreter program."
  :group 'sclang-programs
  :version "21.3"
  :type 'string)

(defcustom sclang-runtime-directory ""
  "*Path to the SuperCollider runtime directory."
  :group 'sclang-options
  :version "21.3"
  :type 'directory
  :options '(:must-match))

(defcustom sclang-library-configuration-file ""
  "*Path of the library configuration file."
  :group 'sclang-options
  :version "21.3"
  :type 'file
  :options '(:must-match))

(defcustom sclang-heap-size ""
  "*Initial heap size."
  :group 'sclang-options
  :version "21.3"
  :type 'string)

(defcustom sclang-heap-growth ""
  "*Heap growth."
  :group 'sclang-options
  :version "21.3"
  :type 'string)

(defcustom sclang-udp-port -1
  "*UDP listening port."
  :group 'sclang-options
  :version "21.3"
  :type 'integer)

(defcustom sclang-main-run nil
  "*Call Main.run on startup."
  :group 'sclang-options
  :version "21.3"
  :type 'boolean)

(defcustom sclang-main-stop nil
  "*Call Main.stop on shutdown."
  :group 'sclang-options
  :version "21.3"
  :type 'boolean)

;; =====================================================================
;; helper functions
;; =====================================================================

(defun sclang-get-process ()
  (get-process sclang-process))

;; =====================================================================
;; library startup/shutdown
;; =====================================================================

(defvar sclang-library-initialized-p nil)

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
;; * sclang opens fifo for communication with emacs during class tree
;;   initialization
;; * sclang sends '_init' command
;; * '_init' command handler calls sclang-on-library-startup to complete
;;   initialization

(defun sclang-library-initialized-p ()
  (and (sclang-get-process)
       sclang-library-initialized-p))

(defun sclang-on-library-startup ()
  (sclang-message "Initializing library...")
  (setq sclang-library-initialized-p t)
  (run-hooks 'sclang-library-startup-hook)
  (sclang-message "Initializing library...done"))

(defun sclang-on-library-shutdown ()
  (when sclang-library-initialized-p
    (run-hooks 'sclang-library-shutdown-hook)
    (setq sclang-library-initialized-p nil)
    (sclang-message "Shutting down library...")))

;; =====================================================================
;; process hooks
;; =====================================================================

(defun sclang-process-sentinel (proc msg)
  (with-sclang-post-buffer
   (goto-char (point-max))
   (insert
    (if (and (bolp) (eolp)) "\n" "\n\n")
    (format "*** %s %s ***" proc (substring msg 0 -1))
    "\n\n"))
  (when (memq (process-status proc) '(exit signal))
    (sclang-on-library-shutdown)
    (sclang-stop-command-process)))

(defun sclang-process-filter (process string)
  (let ((buffer (process-buffer process)))
    (with-current-buffer buffer
      (when (and (> sclang-max-post-buffer-size 0)
		 (> (buffer-size) sclang-max-post-buffer-size))
	(erase-buffer))
      (let ((move-point (or sclang-auto-scroll-post-buffer
			    (= (point) (process-mark process)))))
	(save-excursion
	  ;; replace mac-roman bullet with unicode character
	  (subst-char-in-string sclang-bullet-latin-1 sclang-bullet-utf-8 string t)
	  ;; insert the text, advancing the process marker.
	  (goto-char (process-mark process))
	  (insert string)
	  (set-marker (process-mark process) (point)))
	(when move-point
	  (goto-char (process-mark process))
	  (walk-windows
	   (lambda (window)
	     (when (eq buffer (window-buffer window))
	       (set-window-point window (process-mark process))))
	   nil t))))))

;; =====================================================================
;; process startup/shutdown
;; =====================================================================

(defun sclang-memory-option-p (string)
  (let ((case-fold-search nil))
    (string-match "^[1-9][0-9]*[km]?$" string)))

(defun sclang-port-option-p (number)
  (and (>= number 0) (<= number #XFFFF)))

(defun sclang-make-options ()
  (let ((default-directory "")
	(res ()))
    (flet ((append-option
	    (option &optional value)
	    (setq res (append res (list option) (and value (list value))))))
      (if (file-directory-p sclang-runtime-directory)
	  (append-option "-d" (expand-file-name sclang-runtime-directory)))
      (if (file-exists-p sclang-library-configuration-file)
	  (append-option "-l" (expand-file-name sclang-library-configuration-file)))
      (if (sclang-memory-option-p sclang-heap-size)
	  (append-option "-m" sclang-heap-size))
      (if (sclang-memory-option-p sclang-heap-growth)
	  (append-option "-g" sclang-heap-growth))
      (if (sclang-port-option-p sclang-udp-port)
	  (append-option "-u" (number-to-string sclang-udp-port)))
      (if sclang-main-run
	  (append-option "-r"))
      (if sclang-main-stop
	  (append-option "-s"))
      (append-option "-iscel")
      res)))

(defun sclang-start ()
  "Start SuperCollider process."
  (interactive)
  (sclang-stop)
  (sclang-on-library-shutdown)
  (sit-for 1)
  (sclang-init-post-buffer)
  (sclang-start-command-process)
  (let ((process-connection-type nil))
    (let ((proc (apply 'start-process
		       sclang-process sclang-post-buffer
		       sclang-program (sclang-make-options))))
      (set-process-sentinel proc 'sclang-process-sentinel)
      (set-process-filter proc 'sclang-process-filter)
      (set-process-coding-system proc 'mule-utf-8 'mule-utf-8)
      (set-process-query-on-exit-flag proc nil)
      proc)))

(defun sclang-kill ()
  "Kill SuperCollider process."
  (interactive)
  (when (sclang-get-process)
    (kill-process sclang-process)
    (delete-process sclang-process)))

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
  (sclang-kill)
  (sclang-stop-command-process))

(defun sclang-recompile ()
  "Recompile class library."
  (interactive)
  (when (sclang-get-process)
    (process-send-string sclang-process "\x18")
    ))

;; =====================================================================
;; command process
;; =====================================================================

(defcustom sclang-mkfifo-program "mkfifo"
  "*Name of the \"mkfifo\" program.

Change this if \"mkfifo\" has a non-standard name or location."
  :group 'sclang-programs
  :type 'string)

(defcustom sclang-cat-program "cat"
  "*Name of the \"cat\" program.

Change this if \"cat\" has a non-standard name or location."
  :group 'sclang-programs
  :type 'string)

(defconst sclang-command-process "SCLang Command"
  "Subprocess for receiving command results from sclang.")

(defconst sclang-cmd-helper-proc "SCLang Command Helper"
  "Dummy subprocess that will keep the command fifo open for writing
   so reading does not fail automatically when sclang closes its own
   writing end of the fifo")

(defvar sclang-command-fifo nil
  "FIFO for communicating with the subprocess.")

(defun sclang-delete-command-fifo ()
  (and sclang-command-fifo
       (file-exists-p sclang-command-fifo)
       (delete-file sclang-command-fifo)))

(defun sclang-release-command-fifo ()
  (sclang-delete-command-fifo)
  (setq sclang-command-fifo nil))

(defun sclang-create-command-fifo ()
  (setq sclang-command-fifo (make-temp-name
			      (expand-file-name
			       "sclang-command-fifo." temporary-file-directory)))
  (sclang-delete-command-fifo)
  (let ((res (call-process sclang-mkfifo-program
			   nil t t
			   sclang-command-fifo)))
    (unless (eq 0 res)
      (message "SCLang: Couldn't create command fifo")
      (setq sclang-command-fifo nil))))

(defun sclang-start-command-process ()
  (sclang-create-command-fifo)
  (when sclang-command-fifo
    ;; start the dummy process to keep the fifo open
    (let ((process-connection-type nil))
      (let ((proc (start-process-shell-command
        sclang-cmd-helper-proc nil
        (concat sclang-cat-program " > " sclang-command-fifo))))
	    (set-process-query-on-exit-flag proc nil)))
    ;; sclang gets the fifo path via the environment
    (setenv "SCLANG_COMMAND_FIFO" sclang-command-fifo)
    (let ((process-connection-type nil))
      (let ((proc (start-process
		   sclang-command-process nil
		   sclang-cat-program sclang-command-fifo)))
	(set-process-filter proc 'sclang-command-process-filter)
	;; this is important. use a unibyte stream without eol
	;; conversion for communication.
	(set-process-coding-system proc 'no-conversion 'no-conversion)
	(set-process-query-on-exit-flag proc nil)))
    (unless (get-process sclang-command-process)
      (message "SCLang: Couldn't start command process"))))

(defun sclang-stop-command-process ()
  (when (get-process sclang-cmd-helper-proc)
    (kill-process sclang-cmd-helper-proc)
    (delete-process sclang-cmd-helper-proc))
  ;; the real command process should now quit automatically,
  ;; since there is no more writers to the command fifo
  (sclang-release-command-fifo))

(defvar sclang-command-process-previous nil
  "Unprocessed command process output.")

(defun sclang-command-process-filter (proc string)
  (when sclang-command-process-previous
    (setq string (concat sclang-command-process-previous string)))
  (let (end)
    (while (and (> (length string) 3)
		(>= (length string)
		    (setq end (+ 4 (sclang-string-to-int32 string)))))
      (sclang-handle-command-result (car (read-from-string string 4 end)))
      (setq string (substring string end))))
  (setq sclang-command-process-previous string))

;; =====================================================================
;; command interface
;; =====================================================================

;; symbol property: sclang-command-handler

(defun sclang-set-command-handler (symbol function)
  (put symbol 'sclang-command-handler function))

(defun sclang-perform-command (symbol &rest args)
  (sclang-eval-string (sclang-format
		       "Emacs.lispPerformCommand(%o, %o, true)"
		       symbol args)))

(defun sclang-perform-command-no-result (symbol &rest args)
  (sclang-eval-string (sclang-format
		       "Emacs.lispPerformCommand(%o, %o, false)"
		       symbol args)))

(defun sclang-default-command-handler (fun arg)
  "Default command handler.
Displays short message on error."
  (condition-case nil
      (funcall fun arg)
    (error (sclang-message "Error in command handler") nil)))

(defun sclang-debug-command-handler (fun arg)
  "Debugging command handler.
Enters debugger on error."
  (let ((debug-on-error t)
	(debug-on-signal t))
    (funcall fun arg)))

(defvar sclang-command-handler 'sclang-default-command-handler
  "Function called when handling command result.")

(defun sclang-toggle-debug-command-handler (&optional arg)
  "Toggle debugging of command handler.
With arg, activate debugging iff arg is positive."
  (interactive "P")
  (setq sclang-command-handler
	(if (or (and arg (> arg 0))
		(eq sclang-command-handler 'sclang-debug-command-handler))
	    'sclang-default-command-handler
	  'sclang-default-command-handler))
  (sclang-message "Command handler debugging %s."
		  (if (eq sclang-command-handler 'sclang-debug-command-handler)
		      "enabled"
		    "disabled")))

(defun sclang-handle-command-result (list)
  (condition-case nil
      (let ((fun (get (nth 0 list) 'sclang-command-handler))
	    (arg (nth 1 list))
	    (id  (nth 2 list)))
	(when (functionp fun)
	  (let ((res (funcall sclang-command-handler fun arg)))
	    (when id
	      (sclang-eval-string
	       (sclang-format "Emacs.lispHandleCommandResult(%o, %o)" id res))))))
    (error nil)))

;; =====================================================================
;; code evaluation
;; =====================================================================

(defconst sclang-token-interpret-cmd-line (char-to-string #X1b))
(defconst sclang-token-interpret-print-cmd-line (char-to-string #X0c))

(defcustom sclang-eval-line-forward t
  "*If non-nil `sclang-eval-line' advances to the next line."
  :group 'sclang-interface
  :type 'boolean)

(defun sclang-send-string (token string &optional force)
  (let ((proc (sclang-get-process)))
    (when (and proc (or (sclang-library-initialized-p) force))
      (process-send-string proc (concat string token))
      string)))

(defun sclang-eval-string (string &optional print-p)
  "Send STRING to the sclang process for evaluation and print the result
if PRINT-P is non-nil. Return STRING if successful, otherwise nil."
  (sclang-send-string
   (if print-p sclang-token-interpret-print-cmd-line sclang-token-interpret-cmd-line)
   string))

(defun sclang-eval-expression (string &optional silent-p)
  "Execute STRING as SuperCollider code."
  (interactive "sEval: \nP")
  (sclang-eval-string string (not silent-p)))

(defun sclang-eval-line (&optional silent-p)
  "Execute the current line as SuperCollider code."
  (interactive "P")
  (let ((string (sclang-line-at-point)))
    (when string
      (sclang-eval-string string (not silent-p)))
    (and sclang-eval-line-forward
	 (/= (line-end-position) (point-max))
	 (forward-line 1))
    string))

(defun sclang-eval-region (&optional silent-p)
  "Execute the region as SuperCollider code."
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

(defun sclang-eval-document (&optional silent-p)
  "Execute the whole document as SuperCollider code."
  (interactive "P")
  (save-excursion
    (mark-whole-buffer)
    (sclang-eval-string
     (buffer-substring-no-properties (region-beginning) (region-end))
     (not silent-p))))

(defvar sclang-eval-results nil
  "Save results of sync SCLang evaluation.")

(sclang-set-command-handler
 'evalSCLang
 (lambda (arg) (push arg sclang-eval-results)))

(defun sclang-eval-sync (string)
  "Eval STRING in sclang and return result as a lisp value."
  (let ((proc (get-process sclang-command-process)))
    (if (and (processp proc) (eq (process-status proc) 'run))
	(let ((time (current-time)) (tick 10000) elt)
	  (sclang-perform-command 'evalSCLang string time)
	  (while (and (> (decf tick) 0)
		      (not (setq elt (find time sclang-eval-results
					   :key #'car :test #'equal))))
	    (accept-process-output proc 0 100))
	  (if elt
	      (prog1 (if (eq (nth 1 elt) 'ok)
			 (nth 2 elt)
		       (setq sclang-eval-results (delq elt sclang-eval-results))
		       (signal 'sclang-error (nth 2 elt)))
		(setq sclang-eval-results (delq elt sclang-eval-results)))
	    (error "SCLang sync eval timeout")))
      (error "SCLang Command process not running"))))

;; =====================================================================
;; searching
;; =====================================================================

;; (defun sclang-help-file-paths ()
;;   "Return a list of help file paths."


;; (defun sclang-grep-help-files ()
;;   (interactive)
;;   (let ((sclang-grep-prompt "Search help files: ")
;; 	(sclang-grep-files (mapcar 'cdr sclang-help-topic-alist)))
;;     (call-interactively 'sclang-grep-files)))

;; (defvar sclang-grep-history nil)

;; (defcustom sclang-grep-case-fold-search t
;;   "*Non-nil if sclang-grep-files should ignore case."
;;   :group 'sclang-interface
;;   :version "21.4"
;;   :type 'boolean)

;; (defvar sclang-grep-files nil)
;; (defvar sclang-grep-prompt "Grep: ")

;; (defun sclang-grep-files (regexp)
;;   (interactive
;;    (let ((grep-default (or (when current-prefix-arg (sclang-symbol-at-point))
;; 			   (car sclang-grep-history))))
;;      (list (read-from-minibuffer sclang-grep-prompt
;; 				 grep-default
;; 				 nil nil 'sclang-grep-history))))
;;   (grep-compute-defaults)
;;   (grep (concat grep-program
;; 		" -n"
;; 		(and sclang-grep-case-fold-search " -i")
;; 		" -e" regexp
;; 		" " (mapconcat 'shell-quote-argument sclang-grep-files " "))))

;; =====================================================================
;; workspace
;; =====================================================================

(defcustom sclang-show-workspace-on-startup t
  "*If non-nil show the workspace buffer on library startup."
  :group 'sclang-interface
  :type 'boolean)

(defconst sclang-workspace-buffer (sclang-make-buffer-name "Workspace"))

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
;;	  (insert "// using HTML Help: C-c C-h as usual, then switch to w3m buffer\n")
;;	  (insert "// and do M-x sclang-minor-mode in order te enable sclang code execution\n")
;;	  (insert line)
	  (insert "\n"))
	(set-buffer-modified-p nil)
	;; cwd to sclang-runtime-directory
	(if (and sclang-runtime-directory
		 (file-directory-p sclang-runtime-directory))
	    (setq default-directory sclang-runtime-directory))))
    (switch-to-buffer buffer)))

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

(sclang-set-command-handler '_init (lambda (arg) (sclang-on-library-startup)))

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
	     (cons "-sclang"
		   (lambda (switch)
		     (sclang-start))))

(add-to-list 'command-switch-alist
	     (cons "-sclang-debug"
		   (lambda (switch)
		     (sclang-toggle-debug-command-handler 1))))

(add-to-list 'command-switch-alist
	     (cons "-scmail"
		   (lambda (switch)
		     (sclang-start)
		     (when command-line-args-left
		       (let ((file (pop command-line-args-left)))
			 (with-current-buffer (get-buffer-create sclang-workspace-buffer)
			   (and (file-exists-p file) (insert-file-contents file))
			   (set-buffer-modified-p nil)
			   (sclang-mode)
			   (switch-to-buffer (current-buffer))))))))

(provide 'sclang-interp)

;; EOF
