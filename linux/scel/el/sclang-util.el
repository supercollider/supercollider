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

(defconst sclang-rtf-doc-regexp "{\\\\rtf")
(defconst sclang-rtf-table-regexp (concat
				   "{\\\\\\("
				   (regexp-opt '("colortbl" "filetbl" "fonttbl" "stylesheet")) 
				   "\\)"))
(defconst sclang-rtf-control-regexp "\\\\\\(\\([{}\\\t\n]\\)\\|\\(\\(par\\|tab\\)?[^\\ \n]+ ?\\)\\)")

(defun sclang-un-rtfify-buffer (&optional buffer)
  (with-current-buffer (or buffer (current-buffer))
    (let ((doc-regexp sclang-rtf-doc-regexp)
	  (table-regexp sclang-rtf-table-regexp)
	  (control-regexp sclang-rtf-control-regexp)
	  (case-fold-search nil))
      (save-excursion
	(goto-char (point-min))
	(when (looking-at doc-regexp)
	  (while (re-search-forward "[{}\\\n]" nil t)
	    (goto-char (match-beginning 0))
	    (cond ((looking-at table-regexp)
		   (let ((beg (point)))
		     (forward-list 1)
		     (delete-region beg (point))))
		  ((looking-at control-regexp)
		   (replace-match
		    (or (match-string 2)	;; escape
			(let ((control (match-string 4)))
			  (cond ((string= control "par") "\n")
				((string= control "tab") "\t")
				(t ""))))
		    'fixedcase 'literal))
		  (t (delete-char 1)))))))))

;; EOF