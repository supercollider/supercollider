(defun sclang-make-prompt-string (prompt default)
  (if (and default (string-match "\\(:\\)\\s *" prompt))
      (replace-match
       (format " (default %s):" default)
       'fixedcase 'literal prompt 1)
    prompt))

;; EOF