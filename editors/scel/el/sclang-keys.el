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

;; (defvar sclang-key-table (make-char-table 'foo))

;; (defun sclang-define-key (char beg end)
;;   (interactive)
;;   (sclang-eval-string (sclang-format "Emacs.defineKey(%o, %o)" char code))
;;   (define-key (char-to-string char) sclang-key-mode-map 'sclang-execute-key))

;; (defun sclang-execute-key (char)
;;   (sclang-eval-string (sclang-format "Emacs.executeKey(%o)" char)))

(eval-and-compile (require 'sclang-util)
                  (require 'sclang-interp))

(defun sclang-read-keys ()
  (interactive)
  (let (char)
    (clear-this-command-keys)
    (while t
      (setq char (read-event))
      (clear-this-command-keys)
      (when (characterp char)
	(message "%s (%d)" (char-to-string char) char)
	(sclang-eval-string (format "Emacs.keys.at(%d).value(%d)" char char))))))

;; EOF

