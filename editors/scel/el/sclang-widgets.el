;;; sclang-widgets.el --- Widget definitions for SCLang

;; Copyright (C) 2005  Free Software Foundation, Inc.

;; Author: mlang <mlang@delysid.org>
;; Keywords: comm

;; This file is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; This file is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to
;; the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
;; Boston, MA 02110-1301, USA.

;;; Commentary:

;;

;;; Code:

(eval-when-compile (require 'sclang-util)
                   (require 'sclang-language))
(eval-and-compile (require 'sclang-interp))

(defvar sclang-widgets nil)
(make-variable-buffer-local 'sclang-widgets)

;; Button (not used yet)

(define-widget 'sclang-button 'item
  "A button."
  :create #'sclang-widget-button-create
  :action #'sclang-widget-button-action)

(defun sclang-widget-button-create (widget)
  "Create WIDGET at point in the current buffer."
  (widget-specify-insert
   (let ((from (point))
	 button-begin button-end)
     (setq button-begin (point))
     (insert (widget-get-indirect widget :button-prefix))

     (princ (nth (widget-get widget :value) (widget-get widget :states)) (current-buffer))

     (insert (widget-get-indirect widget :button-suffix))
     (setq button-end (point))

     ;; Specify button, and insert value.
     (and button-begin button-end
	  (widget-specify-button widget button-begin button-end)))
   (let ((from (point-min-marker))
	 (to (point-max-marker)))
     (set-marker-insertion-type from t)
     (set-marker-insertion-type to nil)
     (widget-put widget :from from)
     (widget-put widget :to to)))
  (widget-clear-undo))

(defun sclang-widget-button-action (widget event)
  (widget-value-set widget
		    (if (>= (widget-get widget :value) (1- (length (widget-get widget :states))))
			0
		      (1+ (widget-get widget :value))))
  (sclang-eval-string
   (sclang-format "EmacsWidget.idmap[%o].valueFromEmacs(%o)"
		  (widget-get widget :id) (widget-get widget :value))))

(sclang-set-command-handler
 '_widgetSetStates
 (lambda (arg)
   (multiple-value-bind (buffer id states value) arg
     (with-current-buffer (get-buffer buffer)
       (let ((widget (cdr (find id sclang-widgets :key 'car))))
	 (widget-put widget :states states)
	 (widget-value-set widget value)
	 value)))))

(define-widget 'sclang-slider 'default
  "Slider widget."
  :size 20
  :create #'sclang-widget-slider-create
  :button-prefix "["
  :button-suffix "]"
  :value 0.5
  :value-get #'widget-value-value-get
  :value-set #'sclang-widget-slider-value-set
  :action (lambda (widget event)
	    (let ((pos (if event (posn-point (event-start event)) (point))))
	      (widget-value-set widget (/ (float (- pos (widget-get widget :from))) (widget-get widget :size))))))

(defun sclang-widget-slider-create (widget)
  "Create WIDGET at point in the current buffer."
  (widget-specify-insert
   (let ((from (point))
	 (inhibit-redisplay t)
	 button-begin button-end)
     (setq button-begin (point))
     (insert (widget-get-indirect widget :button-prefix))

     (insert-char ?- (widget-get widget :size))
     (backward-char (1+ (widget-put widget :current-pos (- (widget-get widget :size) (round (* (widget-get widget :value) (widget-get widget :size)))))))
     (delete-char 1) (insert "|")
     (goto-char (point-max))
     (insert (widget-get-indirect widget :button-suffix))
     (setq button-end (point))

     ;; Specify button
     (and button-begin button-end
	  (widget-specify-button widget button-begin button-end)))
   (let ((from (point-min-marker))
	 (to (point-max-marker)))
     (set-marker-insertion-type from t)
     (set-marker-insertion-type to nil)
     (widget-put widget :from from)
     (widget-put widget :to to)))
  (widget-clear-undo))

(defun sclang-widget-slider-value-set (widget value)
  (save-excursion
    (let ((inhibit-read-only t))
      (goto-char (widget-get widget :from))
      (forward-char (widget-get widget :current-pos))
      (insert "-") (delete-char 1)
      (widget-put widget :value value)
      (goto-char (widget-get widget :from))
      (let ((n (round (* value (widget-get widget :size)))))
	(widget-put widget :current-pos n)
	(forward-char n)
	(insert "|") (delete-char 1)))))

;; Class Tree

(require 'tree-widget)
(define-widget 'sclang-class-tree 'tree-widget
  "Widget for displaying the SCLang Class Tree."
  :dynargs #'sclang-widget-class-tree-dynargs)

(defun sclang-widget-class-tree-dynargs (widget)
  (sclang-eval-sync (sclang-format "EmacsClassTree.dynargs(%o)"
				   (widget-get widget :tag))))

(define-widget 'sclang-file-position 'item
  "File position link for the SCLang Class Tree widget."
  :format "%[%t%]\n"
  :action (lambda (widget event)
	    (find-file-other-window (widget-get widget :filename))
	    (goto-char (widget-get widget :char-pos))))

(defun sclang-class-tree (class-name)
  "Display a tree-view of the sub-classes and methods of CLASS-NAME."
  (interactive
   (list (sclang-read-symbol "Class: " "Object" #'sclang-class-name-p)))
  (sclang-eval-string (format "EmacsClassBrowser(%s)" class-name)))

(provide 'sclang-widgets)
;;; sclang-widgets.el ends here
