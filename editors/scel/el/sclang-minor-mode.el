;;; sclang-minor-mode for use in help files
;;; SuperCollider
;;; (c) 2007, Marije Baalman - nescivi <nescivi@gmail.com>
;;;
;;; This program is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;;  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

(eval-when-compile
  (require 'sclang-util)
  (require 'sclang-mode)
  )


(easy-mmode-define-minor-mode sclang-minor-mode
  "Toggle sclang-minor-mode.
With no argument, this command toggles the mode.
Non-null prefix argument turns on the mode.
Null prefix argument turns off the mode.

When sclang-minor-mode is enabled, you can execute
sclang code with the normal command C-c C-c and C-c C-d."
 ;; The initial value.
 nil
 ;; The indicator for the mode line.
 " sclang"
 ;; The minor mode bindings.
 '(("\C-c\C-c" . sclang-eval-region-or-line)
   ("\C-c\C-d" . sclang-eval-region)
   ("\C-\M-x" . sclang-eval-defun)
   ("\C-c\C-h" . sclang-find-help)
   ("\C-\M-h"  . sclang-goto-help-browser)
   ("\C-c\C-s" . sclang-main-stop)
   ("\C-c\C-k" . sclang-edit-dev-source)
))

(provide 'sclang-minor-mode)

(easy-mmode-define-minor-mode sclang-help-minor-mode
  "Toggle sclang-minor-mode.
With no argument, this command toggles the mode.
Non-null prefix argument turns on the mode.
Null prefix argument turns off the mode.

When sclang-help-minor-mode is enabled, you can execute
sclang code with the normal command C-c C-c and C-c C-d."
 ;; The initial value.
 nil
 ;; The indicator for the mode line.
 " sclang-help"
 ;; The minor mode bindings.
 '(("\C-c\C-c" . sclang-eval-region-or-line)
   ("\C-c\C-d" . sclang-eval-region)
   ("\C-\M-x" . sclang-eval-defun)
   ("\C-c\C-h" . sclang-find-help)
   ("\C-c\C-s" . sclang-main-stop)
   ("\C-c\C-v" . sclang-edit-html-help-file)
   ("E" . sclang-edit-help-code)
   ("\C-c\C-k" . sclang-edit-dev-source)
))

(provide 'sclang-help-minor-mode)

(add-hook 'sclang-help-minor-mode-hook 'sclang-minor-hooks)
(add-hook 'sclang-minor-mode-hook 'sclang-minor-hooks)

(defun sclang-minor-hooks ()
  (sclang-init-document)
  (sclang-make-document)
  )
