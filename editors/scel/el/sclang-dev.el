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

