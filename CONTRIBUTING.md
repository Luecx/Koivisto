
1. Depending on the type of change, following applies:
   - A commit which does not change the search or anything related to it, will be accepted as long as it is useful.
   - A commit which does not change the functionality (nodes for bench) but increases the speed, should be tested, depending on the change.
   - A commit which does change the search, eval or anything related to that which does change the bench, must be tested. The SPRT results and bounds must be commented. We recommend SPRT bounds of -[-0.25, 1.25] in which the LLR should be atleast 2.94.
   
 
2. Changes to the code should be commented and explained inside the code.
3. We do not like to copy code from other engines.
4. If there is an idea coming from another engine, this must be stated inside the code.
