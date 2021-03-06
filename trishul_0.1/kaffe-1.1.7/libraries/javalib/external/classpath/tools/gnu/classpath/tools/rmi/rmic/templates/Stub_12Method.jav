  /** @inheritDoc */
  public #return_type #method_name(#argument_list) #throws
  {
    try
      {
        Object result =  ref.invoke(this, met_#method_name,
                   #object_arg_list,
                   #method_hash);
        #return_statement           
      }
    catch (RuntimeException e)
      {
        throw e;
      }
    catch (RemoteException e)
      {
        throw e;
      }
    catch (Exception e)
      {
        UnexpectedException uex = new UnexpectedException(exception_message);
        uex.initCause(e);
        throw uex;
      }
  }
   