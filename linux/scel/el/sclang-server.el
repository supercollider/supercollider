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

(defvar sclang-server-alist nil
  "Alist of currently defined synthesis servers.")

(defvar sclang-current-server nil
  "Currently selected synthesis server.")

(defvar sclang-server-menu (make-sparse-keymap "Servers"))
(defvar sclang-server-history nil)

(defconst sclang-server-running-face (let ((face (make-face 'sclang-server-running-face)))
				       (set-face-foreground face "red")
				       face)
  "")

(sclang-set-command-handler
 '_updateServer
 (lambda (arg)
   (setq sclang-server-alist
	 (sort arg (lambda (a b) (string< (car a) (car b)))))
   (sclang-update-mode-line)))

(defun sclang-get-server (&optional name)
  (unless name (setq name sclang-current-server))
  (cdr (assq name sclang-server-alist)))

(defun sclang-set-server (&optional name)
  (unless name (setq name sclang-current-server))
  (setq sclang-current-server
	(car (or (assq name sclang-server-alist)
		 (car sclang-server-alist)))))

(defun sclang-next-server ()
  (interactive)
  (sclang-set-server)
  (let ((list (or (cdr (member-if (lambda (assoc)
				    (eq (car assoc) sclang-current-server))
				  sclang-server-alist))
		  sclang-server-alist)))
    (setq sclang-current-server (car (car list))))
  (sclang-update-mode-line))
	
(defun sclang-mouse-next-server (event)
  (interactive "e")
  (sclang-next-server))

(defun sclang-server-running-p (&optional name)
  (plist-get (sclang-get-server name) 'running))

(defun sclang-server-booting-p (&optional name)
  (plist-get (sclang-get-server name) 'booting))

(defun sclang-fill-server-menu (map)
  (define-key map [server-quit] (nconc (list 'menu-item "Quit" 'sclang-server-quit (cons nil nil))
				       (list
					:visible '(sclang-server-running-p)
					:help "Quit current server")))
  (define-key map [server-boot] (nconc (list 'menu-item "Boot" 'sclang-server-boot (cons nil nil))
				       (list
					:visible '(not (sclang-server-running-p))
					:help "Boot current server")))
  )

(defun sclang-create-server-menu (title)
  (easy-menu-create-menu
   title
   '(
     ["Boot" sclang-server-boot]
     ["Quit" sclang-server-quit]
     "-"
     ["Free All" sclang-server-free-all :active (sclang-server-running-p)]
     ["Set Latency" sclang-set-server-latency :active (sclang-server-running-p)]
     )))

(defun sclang-server-mode-string ()
  (let* ((name (if sclang-current-server (symbol-name sclang-current-server) "-------"))
	 (server (sclang-get-server))
	 (running-p (if server (plist-get server 'running)))
	 (string (let ((map (make-sparse-keymap))
		       (menu (sclang-create-server-menu "Commands")))
		   ;; 		      (sclang-fill-server-menu menu)
		   (define-key map [mode-line mouse-1] 'sclang-mouse-next-server)
		   (define-key map [mode-line down-mouse-3] menu)
		   ;; (define-key map [down-mouse-1] 'ignore)
		   (propertize
		    name
		    'face (if running-p sclang-server-running-face)
		    'help-echo "mouse-1: next server, mouse-3: command menu"
		    'keymap map)))
	 ;; (make-mode-line-mouse-map 'mouse-1 'sclang-mouse-next-server)))
	 (address (if (and server (not (eq (plist-get server 'type) 'internal)))
		      (format " (%s)" (plist-get server 'address))
		    ""))
	 (info (if running-p
		   (mapcar 'number-to-string
			   (plist-get (sclang-get-server) 'info))
		 '("---" "---" "----" "----" "----" "----"))))
    (apply 'format "%s%s %3s|%3s %% u: %4s s: %4s g: %4s d: %4s" string address info)))

(defun sclang-update-mode-line ()
  (interactive)
  (sclang-set-server)
  (with-sclang-post-buffer
   (setq sclang-post-buffer-mode-string (sclang-server-mode-string))
   (force-mode-line-update)))

(add-hook 'sclang-library-startup-hook 'sclang-update-mode-line)

;; =====================================================================
;; language control
;; =====================================================================

(defun sclang-perform-server-command (command &rest args)
  (sclang-eval-string
   (sclang-format "Server.named.at(%o.asSymbol).performList(\\tryPerform, %o.asSymbol.asArray ++ %o)"
		  sclang-current-server command args)
   nil))

(defun sclang-server-boot ()
  (interactive)
  (sclang-perform-server-command "boot"))

(defun sclang-server-reboot ()
  (interactive)
  (sclang-perform-server-command "reboot"))

(defun sclang-server-quit ()
  (interactive)
  (sclang-perform-server-command "quit"))

(defun sclang-server-free-all ()
  (interactive)
  (sclang-perform-server-command "freeAll"))

(defun sclang-set-server-latency (lat)
  (interactive "nSet latency: ")
  (sclang-perform-server-command "latency_" lat))

(defun sclang-show-server-latency ()
  (interactive)
  (let ((server (sclang-get-server)))
    (message "%s" (and server (plist-get server 'latency)))))

;; =====================================================================
;; module setup
;; =====================================================================

(provide 'sclang-server)

;; EOF