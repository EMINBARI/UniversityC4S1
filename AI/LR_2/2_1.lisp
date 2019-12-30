(defun myfunction (mylist)
  (cond
   ((null mylist) nil)
   (t (nconc (reverse mylist) (myfunction (rest mylist))))
  )
)