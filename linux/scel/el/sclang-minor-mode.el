;;; sclang-minor-mode for use in help files
;;; SuperCollider
;;; (c) 2007, Marije Baalman - nescivi
;;; released under GPL

(easy-mmode-define-minor-mode sclang-minor-mode
  "Toggle sclang-minor-mode.
With no argument, this command toggles the mode. 
Non-null prefix argument turns on the mode.
Null prefix argument turns off the mode.

When sclang-minor-mode is enabled, you can execute
sclang code with the normal command C-c C-c and C-c C-x."
 ;; The initial value.
 nil
 ;; The indicator for the mode line.
 " sclang"
 ;; The minor mode bindings.
 '(("\C-c\C-c" . sclang-eval-region-or-line)
   ("\C-c\C-x" . sclang-eval-region)
   ("\C-c\C-h" . sclang-find-help)
))

(provide 'sclang-minor-mode)