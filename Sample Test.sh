rm -f lan?  hout?  hin?
rm -f rout?
host 0 0 sender 50 20&
host 1 1 receiver &
router 0 0 1 &
router 1 1 2 &
router 2 2 3 &
router 3 3 0 &
controller host 0 1 router 0 1 2 3 lan 0 1 2 3&
