(defun sclang-message (string &rest args)
  (message "SCLang: %s" (apply #'format string args)))

(defun sclang-make-prompt-string (prompt default)
  (if (and default (string-match "\\(:\\)\\s *" prompt))
      (replace-match
       (format " (default %s):" default)
       'fixedcase 'literal prompt 1)
    prompt))

(defun sclang-string-to-int32 (str)
  "Convert first 4 bytes of str (network byteorder) to 32 bit integer."
  (logior (lsh (logand (aref str 0) #XFF) 24)
	  (lsh (logand (aref str 1) #XFF) 16)
	  (lsh (logand (aref str 2) #XFF) 8)
	  (logand (aref str 3) #XFF)))

(defun sclang-int32-to-string (n)
  "Convert 32 bit integer n to 4 byte string (network byte order)."
  (let ((str (make-string 4 0)))
    (aset str 0 (logand (lsh n -24) #XFF))
    (aset str 1 (logand (lsh n -16) #XFF))
    (aset str 2 (logand (lsh n  -8) #XFF))
    (aset str 3 (logand          n  #XFF))
    str))

(defun sclang-compress-newlines (&optional buffer)
  (with-current-buffer (or buffer (current-buffer))
    (save-excursion
      (goto-char (point-min))
      (while (not (eobp))
	(if (and (bolp) (eolp)
		 (save-excursion
		   (forward-line -1)
		   (and (bolp) (eolp))))
	    (delete-char 1)
	  (forward-line 1))))))

(eval-when-compile
  (defmacro sclang-save-buffer-state (varlist &rest body)
    "Bind variables according to VARLIST and eval BODY restoring buffer state."
    `(let* ,(append varlist
		    '((modified (buffer-modified-p)) (buffer-undo-list t)
		      (inhibit-read-only t) (inhibit-point-motion-hooks t)
		      (inhibit-modification-hooks t)
		      deactivate-mark buffer-file-name buffer-file-truename))
       (unwind-protect
	   ,@body
	 (when (and (not modified) (buffer-modified-p))
	   (set-buffer-modified-p nil))))))

;; (defun sclang-create-image (file-name &rest props)
;;   (when (file-exists-p file-name)
;;     (let ((coding-system-for-read 'no-conversion)
;; 	  (coding-system-for-write 'no-conversion)
;; 	  (inhibit-quit t))
;;       (with-temp-buffer
;; 	(when (equal 0 (call-process "anytopnm" file-name (list (current-buffer) nil)))
;; 	  (apply
;; 	   'create-image
;; 	   (buffer-substring-no-properties (point-min) (point-max))
;; 	   nil t props))))))

(provide 'sclang-util)

;; EOF