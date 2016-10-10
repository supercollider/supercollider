;;; sclang-vars.el --- Variables with build-time defaults

;; Copyright (C) 2005  Free Software Foundation, Inc.

;; Author: Mario Lang <mlang@delysid.org>

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

;;; Code:

(defconst sclang-system-data-dir "@PKG_DATA_DIR@"
  "Installation dependent data directory.")

(defconst sclang-system-help-dir "@PKG_DATA_DIR@/Help"
  "Installation dependent help directory.")

(defconst sclang-system-extension-dir "@PKG_DATA_DIR@/Extensions"
  "Installation dependent extension directory.")

(provide 'sclang-vars)
;;; sclang-vars.el ends here
