(defun rfun (mylist)
  (do ( (rlist mylist (rest rlist))
        (result ())
      )
      ((null rlist) result)
      (setf result (nconc result (reverse rlist)))
  )
)

