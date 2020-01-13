package trishul_p;

public class InsertOrder
extends ResultOrder
{
	private ConcreteAction insertAction;

	public InsertOrder (ConcreteAction insertAction)
	{
		this (insertAction, null);
	}

	public InsertOrder (ConcreteAction insertAction, ResultHandler handler)
  	{
  		super (handler);
  		this.insertAction = insertAction;
  	}

  	public ConcreteAction getInsertAction()
  	{
  		return insertAction;
  	}

	public int getType ()
	{
		return TYPE_INSERT;
	}
}

