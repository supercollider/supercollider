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
  (require 'sclang-util)
  (require 'sclang-interp)
  )



(sclang-set-command-handler
  'openDevSource
  (lambda (file)
   )
  )

(defun sclang-edit-dev-source ()
  "Edit the help file at the development location."
;  (sclang-document-name . (prSetTitle (buffer-name)))
  (interactive)
  (sclang-perform-command 'openDevSource (buffer-file-name))
  )

(provide 'sclang-dev)

;(defun sclang-open-dev-source (file)
;  "Open the help file at the development location."
;  (if (sclang-html-file-p file)
;      (html-mode)
;    ;; (find-file file)
;    )
;  (if ( sclang-sc-file-p file )
;      (sclang-mode)
;    )
;  )

