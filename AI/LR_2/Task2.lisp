(defun f (sp obj1 obj2)
 
  (let ((newsp '()))
    (dolist 
      (i sp newsp) 
      (if (eql obj1 i) (setf (nth (- i 1) sp) obj2))
    )
  )
)
 (f '(1 2 3) 2 8)

 
