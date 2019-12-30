(defun f1 (a b c)
  (defparameter array (list a b c))
  
  (defparameter i (second array))

  (print array)
  (cond
        ((integerp i) (butlast array))
        ((floatp i) (* i i))
        ((rationalp i) (* i i))
        
        (T (butlast array))
  )
)

