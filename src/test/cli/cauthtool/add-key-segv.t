  $ cauthtool kring --create-keyring
  creating kring

# TODO fix me
  $ cauthtool kring --add-key 'FAKEBASE64 foo'
  *** Caught signal (Segmentation fault) ***
  in thread [0-9a-f]{12} (re)
   ceph version .* (re)
   1: .* (re)
   2: .* (re)
   3: .* (re)
   4: .* (re)
   5: .* (re)
   6: .* (re)
   7: .* (re)
   8: .* (re)
  Segmentation fault
  [139]