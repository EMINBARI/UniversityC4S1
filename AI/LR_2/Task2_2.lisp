(defun rpl (sp obj1 obj2)
  (let ((newsp sp))
    (dolist
      (i sp)
      (if (eql i obj1) (nsubstitute obj2 i newsp))
    )
    (print newsp)
  )
)

(rpl '(1 2 3 4 2 2) 2 8)