using System;

namespace OverCraft.Data {
	public class Dice {

		protected Random _dice;

		public Dice() {
			_dice = new Random();
		}

		public int Throw() {
			return _dice.Next(1, 7);
		}
	}
}
