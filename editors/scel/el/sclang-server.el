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
  (require 'sclang-interp)
  (require 'sclang-language)
  (require 'sclang-mode))

(defcustom sclang-server-panel "Server.default.makeWindow"
  "*Expression to execute when `sclang-show-server-panel' is invoked."
  :group 'sclang-interface
  :type '(choice (const "Server.default.makeWindow")
		 (const "\\SCUM.asClass.do { \\SCUM.asClass.desktop.showServerPanel }")
		 string))

(defvar sclang-server-alist nil
  "Alist of currently defined synthesis servers.")

(defvar sclang-default-server nil
  "Default synthesis server.")

(defvar sclang-current-server nil
  "Currently selected synthesis server.")

(defvar sclang-current-server-initialized nil
  "Non nil when the current server has been initialized from the default server.")

(defconst sclang-server-running-face
  (let ((face (make-face 'sclang-server-running-face)))
    (set-face-foreground face "red")
    face)
  "Face for highlighting a server's running state in the mode-line.")

(defun sclang-get-server (&optional name)
  (unless name (setq name sclang-current-server))
  (cdr (assq name sclang-server-alist)))

(defun sclang-set-server (&optional name)
  (unless name (setq name sclang-current-server))
  (setq sclang-current-server
	(car (or (assq name sclang-server-alist)
		 (car sclang-server-alist)))))

(sclang-set-command-handler
 '_updateServer
 (lambda (arg)
   (setq sclang-server-alist
	 (sort (cdr arg) (lambda (a b) (string< (car a) (car b)))))
   (setq sclang-default-server (car arg))
   (unless sclang-current-server-initialized
     ;; only set the current server automatically once after startup
     (setq sclang-current-server-initialized t)
     (sclang-set-server sclang-default-server))
   (sclang-update-server-info)))

(defun sclang-next-server ()
  "Select next server for display."
  (interactive)
  (sclang-set-server)
  (let ((list (or (cdr (member-if (lambda (assoc)
				    (eq (car assoc) sclang-current-server))
				  sclang-server-alist))
		  sclang-server-alist)))
    (setq sclang-current-server (car (car list))))
  (sclang-update-server-info))

(defun sclang-mouse-next-server (event)
  "Select next server for display."
  (interactive "e")
  (sclang-next-server))

(defun sclang-server-running-p (&optional name)
  (plist-get (sclang-get-server name) 'running))

(defun sclang-server-booting-p (&optional name)
  (plist-get (sclang-get-server name) 'booting))

(defun sclang-create-server-menu (title)
  (easy-menu-create-menu
   title
   '(
     ["Boot" sclang-server-boot]
     ["Quit" sclang-server-quit]
     "-"
     ["Free All" sclang-server-free-all :active (sclang-server-running-p)]
     ["Make Default" sclang-server-make-default]
     )))

(defun sclang-server-fill-mouse-map (map prefix)
  (define-key map (vector prefix 'mouse-1) 'sclang-mouse-next-server)
  (define-key map (vector prefix 'down-mouse-3) (sclang-create-server-menu "Commands"))
  map)

(defvar sclang-server-mouse-map (sclang-server-fill-mouse-map (make-sparse-keymap) 'mode-line)
  "Keymap used for controlling servers in the mode line.")

(defun sclang-server-fill-key-map (map)
  "Fill server prefix map."
  (define-key map [?b] 'sclang-server-boot)
  (define-key map [?d] 'sclang-server-display-default)
  (define-key map [?f] 'sclang-server-free-all)
  (define-key map [?m] 'sclang-server-make-default)
  (define-key map [?n] 'sclang-next-server)
  (define-key map [?o] 'sclang-server-dump-osc)
  (define-key map [?p] 'sclang-show-server-panel)
  (define-key map [?q] 'sclang-server-quit)
  (flet ((fill-record-map (map)
			  (define-key map [?n] 'sclang-server-prepare-for-record)
			  (define-key map [?p] 'sclang-server-pause-recording)
			  (define-key map [?r] 'sclang-server-record)
			  (define-key map [?s] 'sclang-server-stop-recording)
			  map))
    (define-key map [?r] (fill-record-map (make-sparse-keymap))))
  map)

(defvar sclang-server-key-map (sclang-server-fill-key-map (make-sparse-keymap))
  "Keymap used for controlling servers.")

(defun sclang-get-server-info-string ()
  "Return a mode-line string for the current server."
  (let* ((name (if sclang-current-server (symbol-name sclang-current-server) "-------"))
	 (server (sclang-get-server))
	 (running-p (if server (plist-get server 'running)))
	 (string (propertize
		  name
		  'face (if running-p sclang-server-running-face)
		  'help-echo "mouse-1: next server, mouse-3: command menu"
		  'keymap sclang-server-mouse-map))
	 ;; (make-mode-line-mouse-map 'mouse-1 'sclang-mouse-next-server)))
	 (address (if (and server (not (eq (plist-get server 'type) 'internal)))
		      (format " (%s)" (plist-get server 'address))
		    ""))
	 (info (if running-p
		   (mapcar 'number-to-string
			   (plist-get (sclang-get-server) 'info))
		 '("---" "---" "----" "----" "----" "----"))))
    (apply 'format "%s%s %3s|%3s %% u: %4s s: %4s g: %4s d: %4s" string address info)))

(defvar sclang-server-info-string (sclang-get-server-info-string)
  "Info string used in the post buffer mode line.")

(defun sclang-update-server-info ()
  (interactive)
  (sclang-set-server)
  (setq sclang-server-info-string (sclang-get-server-info-string))
  (force-mode-line-update t))

;; =====================================================================
;; language control
;; =====================================================================

(defun sclang-perform-server-command (command &rest args)
  (sclang-eval-string
   (sclang-format "Server.named.at(%o.asSymbol).performList(\\tryPerform, %o.asSymbol.asArray ++ %o)"
		  sclang-current-server command args)
   nil))

(defun sclang-server-boot ()
  "Boot the current server."
  (interactive)
  (sclang-perform-server-command "boot"))

(defun sclang-server-reboot ()
  "Reboot the current server."
  (interactive)
  (sclang-perform-server-command "reboot"))

(defun sclang-server-quit ()
  "Quit the current server."
  (interactive)
  (sclang-perform-server-command "quit"))

(defun sclang-server-free-all ()
  "Free all nodes on the current server."
  (interactive)
  (sclang-perform-server-command "freeAll"))

(defun sclang-server-display-default ()
  "Display default server."
  (interactive)
  (when sclang-default-server
    (sclang-set-server sclang-default-server)
    (sclang-update-server-info)))

(defun sclang-server-make-default ()
  "Make current server the default server."
  (interactive)
  (when sclang-current-server
    (sclang-eval-string
     (sclang-format "
var server;
server = Server.named.at(%o);
if (server.notNil) {
    Server.default = server;
    thisProcess.interpreter.s = server;
    \"Default server: %\n\".postf(server.name);
};
" sclang-current-server))
    nil))

(defun sclang-server-dump-osc (&optional code)
  "Set the current server's dump OSC mode."
  (interactive "P")
  (sclang-perform-server-command "dumpOSC"
				 (cond ((consp code) 0)
				       ((null code) 1)
				       (t code))))

(defun sclang-server-prepare-for-record (&optional path)
  "Prepare current server for recording a sound file."
  (interactive
   (list
    (and current-prefix-arg (read-file-name "Record to file: "))))
  (sclang-perform-server-command "prepareForRecord" path))

(defun sclang-server-record ()
  "Record a sound file on the current server."
  (interactive)
  (sclang-perform-server-command "record"))

(defun sclang-server-pause-recording ()
  "Pause recording on the current server."
  (interactive)
  (sclang-perform-server-command "pauseRecording"))

(defun sclang-server-stop-recording ()
  "Stop recording on the current server."
  (interactive)
  (sclang-perform-server-command "stopRecording"))

(defun sclang-set-server-latency (lat)
  "Set the current server's `latency' instance variable."
  (interactive "nSet latency: ")
  (sclang-perform-server-command "latency_" lat))

(defun sclang-show-server-latency ()
  "Show the current server's latency."
  (interactive)
  (let ((server (sclang-get-server)))
    (message "%s" (and server (plist-get server 'latency)))))

(defun sclang-show-server-panel ()
  "Show graphical server panel if available."
  (interactive)
  (sclang-eval-string sclang-server-panel))

;; =====================================================================
;; module setup
;; =====================================================================

(add-hook 'sclang-mode-hook
	  (lambda ()
	    ;; install server mode line in post buffer
	    (when (string= (buffer-name) sclang-post-buffer)
	      (setq mode-line-format '("-" sclang-server-info-string)))
	    ;; install server prefix keymap
	    (define-key sclang-mode-map "\C-c\C-p" sclang-server-key-map)))

(add-hook 'sclang-library-shutdown-hook
	  (lambda ()
	    (setq sclang-current-server-initialized nil)))

(provide 'sclang-server)

;; EOF
