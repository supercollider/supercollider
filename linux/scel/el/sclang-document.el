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

;; =====================================================================
;; document interface
;; =====================================================================

(defvar sclang--current-document nil
  "Currently active document.")

(defun sclang-document-name (&optional buffer)
  (buffer-name buffer))

(defun sclang-document-p (&optional buffer)
  (with-current-buffer (or buffer (current-buffer))
    (and (boundp 'sclang-document-p) sclang-document-p)))

(defun sclang-get-document (name)
  (let ((buffer (get-buffer name)))
    (and buffer (sclang-document-p buffer) buffer)))

(defun sclang-make-document (&optional buffer)
  (with-current-buffer (or buffer (current-buffer))
    (set (make-local-variable 'sclang-document-p) t)
    (sclang-perform-command-no-result
     'documentNew
     (sclang-document-name)
     (buffer-file-name))))

(defun sclang-make-buffer-current (&optional buffer)
  (let ((buffer (or buffer (current-buffer))))
    (unless (eq buffer sclang--current-document)
      (let ((prev-doc sclang--current-document)
	    (cur-doc (and (sclang-document-p buffer) buffer)))
	(if prev-doc
	    (sclang-perform-command-no-result 'documentBecomeKey (sclang-document-name prev-doc) nil))
	(if cur-doc
	    (sclang-perform-command-no-result 'documentBecomeKey (sclang-document-name cur-doc) t))
	(setq sclang--current-document cur-doc)))))

(defun sclang-document-list ()
  (remove-if-not #'sclang-document-p (buffer-list)))

(defun sclang--document-library-startup-function ()
  (dolist (buffer (sclang-document-list))
    (sclang-make-document buffer))
  (sclang-make-buffer-current (current-buffer)))

(defun sclang--document-kill-buffer-hook-function ()
  (when (sclang-document-p)
    (sclang-perform-command-no-result 'documentClosed (sclang-document-name))))

(defun sclang--document-post-command-hook-function ()
  (sclang-make-buffer-current))

;; =====================================================================
;; commands
;; =====================================================================

(sclang-set-command-handler
 '_documentNew
 (lambda (arg)
   ))

(sclang-set-command-handler
 '_documentOpen
 (lambda (arg)
   ))

(sclang-set-command-handler
 '_documentClose
 (lambda (arg)
   (let ((doc (and (stringp arg) (sclang-get-document arg))))
     (and doc (kill-buffer doc)))))

(sclang-set-command-handler
 '_documentSwitchTo
 (lambda (arg)
   (let ((doc (and (stringp arg) (sclang-get-document arg))))
     (and doc (switch-to-buffer doc)))))

(sclang-set-command-handler
 '_documentPopTo
 (lambda (arg)
   (let ((doc (and (stringp arg) (sclang-get-document arg))))
     (and doc (display-buffer doc)))))

;; =====================================================================
;; module setup
;; =====================================================================

(add-hook 'sclang-mode-hook #'sclang-make-document)
(add-hook 'sclang-library-startup-hook #'sclang--document-library-startup-function)
(add-hook 'kill-buffer-hook #'sclang--document-kill-buffer-hook-function)
;; post-command-idle-hook
(add-hook 'post-command-hook #'sclang--document-post-command-hook-function)

(provide 'sclang-document)

;; EOF